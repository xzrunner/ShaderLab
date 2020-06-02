#include <sx/ResFileHelper.h>
#include <ns/NodeFactory.h>
#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <unirender/Factory.h>
#include <ns/RegistCallback.h>
#include <facade/Facade.h>
#include <shadergraph/Evaluator.h>
#include <shadergraph/Block.h>
#include <shadergraph/ShaderGraph.h>
#include <shadergraph/block/FragmentShader.h>
#include <js/RapidJsonHelper.h>
#include <dag/Node.h>
#include <unirender/typedef.h>
#include <unirender/Device.h>
#include <unirender/ShaderProgram.h>
#include <unirender/TextureDescription.h>
#include <unirender/Framebuffer.h>
#include <unirender/Context.h>
#include <unirender/DrawState.h>
#include <unirender/ClearState.h>
#include <gimg_export.h>
#include <gimg_typedef.h>

#include <blueprint/Node.h>
#include <blueprint/Pin.h>
#include <blueprint/Connecting.h>
#include <blueprint/Blueprint.h>
#include <blueprint/CompNode.h>
#include <blueprint/NSCompNode.h>
#include <blueprint/SerializeHelper.h>
#include <shaderlab/ShaderLab.h>
#include <shaderlab/ShaderAdapter.h>
#include <shaderlab/Evaluator.h>
#include <shaderlab/Node.h>
#include <shaderlab/node/Texture2DAsset.h>

#include <gl/glew.h>
#include <glfw3.h>

#include <boost/filesystem.hpp>

#include <stdio.h>
#include <stdlib.h>

namespace
{

const char* vs = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
	gl_Position = vec4(aPos, 0.0, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
)";

const int TEX_SIZE = 512;

uint8_t* BUFFER = nullptr;

}

void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

bool init_gl()
{
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	GLFWwindow* window = glfwCreateWindow(100, 100, "rotate-crop", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	//// Initialize GLEW to setup the OpenGL Function pointers
	//if (glewInit() != GLEW_OK) {
	//	return -1;
	//}

	return true;
}

std::shared_ptr<ur::ShaderProgram>
build_shader(const ur::Device& dev, const std::vector<bp::NodePtr>& front_nodes,
             const std::vector<std::shared_ptr<dag::Node<shadergraph::Variant>>>& back_nodes,
             std::vector<std::pair<size_t, ur::TexturePtr>>& textures)
{
    shadergraph::Evaluator eval;

    std::string fs;

    // build frag m_shader
    for (auto& back : back_nodes)
    {
        if (!back) {
            continue;
        }
        auto block = std::static_pointer_cast<shadergraph::Block>(back);
        if (block->get_type() == rttr::type::get<shadergraph::block::FragmentShader>())
        {
            eval.Rebuild(block);
            fs = eval.GenShaderCode();
            break;
        }
    }

    if (fs.empty()) {
        return nullptr;
    }

    auto shader = dev.CreateShaderProgram(vs, fs);
    if (!shader->CheckStatus()) {
        return nullptr;
    }

    // textures
    assert(front_nodes.size() == back_nodes.size());
    for (int i = 0, n = front_nodes.size(); i < n; ++i)
    {
        auto& front = front_nodes[i];
        auto& back = back_nodes[i];
        if (front->get_type() != rttr::type::get<shaderlab::node::Texture2DAsset>()) {
            continue;
        }

        auto tex = std::static_pointer_cast<shaderlab::node::Texture2DAsset>(front)->GetTexture();
        if (!tex) {
            continue;
        }

        assert(back);
        auto block = std::static_pointer_cast<shadergraph::Block>(back);
        auto name = eval.QueryRealName(&block->GetExports()[0].var.type);

        auto slot = shader->QueryTexSlot(name);
        if (slot >= 0) {
            textures.push_back({ slot, tex });
        }
    }

    // update uniforms
    shaderlab::Evaluator::UpdateUniforms(eval, shader);

    return shader;
}

void test_file(const ur::Device& dev, ur::Context& ctx,
               const std::shared_ptr<ur::Framebuffer>& rt,
               const std::string& filepath)
{
    auto dir = boost::filesystem::path(filepath).parent_path().string();

    auto root = ns::NodeFactory::Create(dev, filepath);

    auto& ccomplex = root->GetSharedComp<n0::CompComplex>();
    auto& children = ccomplex.GetAllChildren();

    std::vector<bp::NodePtr> front_nodes(children.size(), nullptr);
    std::vector<std::shared_ptr<dag::Node<shadergraph::Variant>>> back_nodes(children.size(), nullptr);

    bp::SerializeHelper::SetupNodes(children, front_nodes, back_nodes);
    assert(front_nodes.size() == back_nodes.size());
    for (int i = 0, n = front_nodes.size(); i < n; ++i) {
        shaderlab::ShaderAdapter::Front2Back(*front_nodes[i], *back_nodes[i], dir, dev);
    }
    bp::SerializeHelper::SetupConnections(filepath, children, front_nodes, back_nodes);

    std::vector<std::pair<size_t, ur::TexturePtr>> textures;
    auto shader = build_shader(dev, front_nodes, back_nodes, textures);

    ctx.SetFramebuffer(rt);
    ctx.SetViewport(0, 0, TEX_SIZE, TEX_SIZE);

    ur::ClearState clear;
    clear.buffers = ur::ClearBuffers::ColorAndDepthBuffer;
    clear.color.FromRGBA(0x88888888);
    ctx.Clear(clear);

    ur::DrawState ds;
    ds.program      = shader;
    ds.render_state = ur::DefaultRenderState2D();
    ds.vertex_array = dev.GetVertexArray(ur::Device::PrimitiveType::Quad, ur::VertexLayoutType::PosTex);

    for (auto& t : textures) {
        ctx.SetTexture(t.first, t.second);
    }

    ctx.Draw(ur::PrimitiveType::TriangleStrip, ds, nullptr);

    dev.ReadPixels(BUFFER, ur::TextureFormat::RGB, 0, 0, TEX_SIZE, TEX_SIZE);

    auto out_filepath = filepath.substr(0, filepath.find_last_of('.')) + ".jpg";
    gimg_export(out_filepath.c_str(), BUFFER, TEX_SIZE, TEX_SIZE, GPF_RGB, false);

    ctx.SetFramebuffer(nullptr);
}

void test_folder(const ur::Device& dev, ur::Context& ctx,
                 const std::string& dir)
{
    ur::TextureDescription desc;
    desc.target = ur::TextureTarget::Texture2D;
    desc.width  = TEX_SIZE;
    desc.height = TEX_SIZE;
    desc.format = ur::TextureFormat::RGB;
    auto tex = dev.CreateTexture(desc);

    auto fbo = dev.CreateFramebuffer();
    fbo->SetAttachment(ur::AttachmentType::Color0, ur::TextureTarget::Texture2D, tex, nullptr);

    boost::filesystem::recursive_directory_iterator itr(dir), end;
    while (itr != end)
    {
        if (boost::filesystem::is_regular(itr->path()))
        {
            auto path = itr->path().string();
            if (sx::ResFileHelper::Type(path) == sx::ResFileType::RES_FILE_JSON) {
                test_file(dev, ctx, fbo, path);
            }
        }
        ++itr;
    }
}

int main()
{
    init_gl();

    auto dev = ur::CreateDeviceGL();
    auto ctx = ur::CreateContextGL(*dev);

    ns::RegistCallback::Init();
    facade::Facade::Instance()->Init(*dev);
    bp::Blueprint::Instance();
    shadergraph::ShaderGraph::Instance();
    shaderlab::ShaderLab::Instance();

    BUFFER = new uint8_t[TEX_SIZE * TEX_SIZE * 4];

    // fixme: id
    auto node = std::make_shared<n0::SceneNode>();
    node->AddSharedComp<n0::CompComplex>();

    test_folder(*dev, *ctx, "../../../test/cases");

    return 0;
}