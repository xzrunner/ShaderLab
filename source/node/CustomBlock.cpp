#include "shaderlab/node/CustomBlock.h"

#include <blueprint/BackendAdapter.h>

#include <shadergraph/Block.h>

namespace shaderlab
{
namespace node
{

void CustomBlock::SetCode(const std::string& code)
{ 
    m_code = code; 

    shadergraph::Block block(code);

    // pins
    bp::BackendAdapter<shadergraph::Variant>
        trans("shadergraph", BackToFront);
    trans.InitNodePins(*this, block);
    
    // props
    InitProps(block.GetVariants());
}

}
}