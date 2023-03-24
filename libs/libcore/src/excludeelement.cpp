#include "excludeelement.h"

ExcludeElement::ExcludeElement() : Element()
{
	_operator=nullptr;
}

void ExcludeElement::setOperator(Operator *oper)
{
	_operator=oper;
}

Operator *ExcludeElement::getOperator()
{
	return _operator;
}

QString ExcludeElement::getSourceCode(SchemaParser::CodeType def_type)
{
	attribs_map attributes;

	schparser.setPgSQLVersion(BaseObject::getPgSQLVersion());
	attributes[Attributes::Operator]="";
	configureAttributes(attributes, def_type);

	if(_operator)
	{
		if(def_type==SchemaParser::SqlCode)
			attributes[Attributes::Operator]=_operator->getName(true);
		else
			attributes[Attributes::Operator]=_operator->getSourceCode(def_type, true);
	}

	return schparser.getSourceCode(Attributes::ExcludeElement, attributes, def_type);
}

bool ExcludeElement::operator == (ExcludeElement &elem)
{
	return (this->_operator==elem._operator &&
					*(dynamic_cast<Element *>(this))==dynamic_cast<Element &>(elem));
}
