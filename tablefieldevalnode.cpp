#include "tablefieldevalnode.h"

TableFieldEvalNode::TableFieldEvalNode(QObject *parent) :
    QObject(parent), m_type(TableFieldEvalNode::NO_TYPE), m_priority(0)
{
}

TableFieldEvalNode::TableFieldEvalNode(const TableFieldEvalNode& obj, QObject *parent) :
    QObject(parent), m_type(TableFieldEvalNode::NO_TYPE), m_priority(0)
{
    operator=(obj);
}

TableFieldEvalNode::TableFieldEvalNode(const OperatorType oType, QObject *parent) :
    QObject(parent), m_type(oType), m_priority(0)
{
    setPriority(operatorPriority(oType));
}

TableFieldEvalNode::TableFieldEvalNode(const OperatorType oType, const QString& value, QObject *parent) :
    QObject(parent), m_type(oType), m_priority(0), m_value(value)
{
    setPriority(operatorPriority(oType));
}


TableFieldEvalNode* TableFieldEvalNode::createAnd(QObject *parent)
{
    return new TableFieldEvalNode(TableFieldEvalNode::AND, parent);
}

TableFieldEvalNode* TableFieldEvalNode::createOr(QObject *parent)
{
    return new TableFieldEvalNode(TableFieldEvalNode::OR, parent);
}

TableFieldEvalNode* TableFieldEvalNode::createNot(QObject *parent)
{
    return new TableFieldEvalNode(TableFieldEvalNode::NOT, parent);
}

TableFieldEvalNode* TableFieldEvalNode::createLeftParen(QObject *parent)
{
    return new TableFieldEvalNode(TableFieldEvalNode::L_PAREN, parent);
}

TableFieldEvalNode* TableFieldEvalNode::createRightParen(QObject *parent)
{
    return new TableFieldEvalNode(TableFieldEvalNode::R_PAREN, parent);
}

TableFieldEvalNode* TableFieldEvalNode::createValue(const QString& value, QObject *parent)
{
    return new TableFieldEvalNode(TableFieldEvalNode::VALUE, value, parent);
}

TableFieldEvalNode* TableFieldEvalNode::clone(QObject *parent)
{
    return new TableFieldEvalNode(*this, parent);
}

const TableFieldEvalNode& TableFieldEvalNode::operator=(const TableFieldEvalNode& obj)
{
    if (this != &obj)
    {
        m_type = obj.m_type;
        m_priority = obj.m_priority;
        m_value = obj.m_value;
    }
    return *this;
}

int TableFieldEvalNode::operatorPriority(const OperatorType oType)
{
    switch (oType)
    {
    case TableFieldEvalNode::NOT :
        return 80;
        break;
    case TableFieldEvalNode::AND :
        return 20;
        break;
    case TableFieldEvalNode::OR :
        return 10;
        break;
    case TableFieldEvalNode::L_PAREN :
        return 99;
        break;
    case TableFieldEvalNode::R_PAREN :
        return 99;
        break;
     default:
        break;
    }
    return 0;
}

void TableFieldEvalNode::setType(const OperatorType aType, const QString& aValue, bool fillRest)
{
  setType(aType, fillRest);
  setValue(aValue);
}

void TableFieldEvalNode::setType(const OperatorType aType, bool fillRest)
{
  m_type = aType;
  if (fillRest)
  {
    setValue("");
    setPriority(operatorPriority(aType));
    switch (aType)
    {
    case TableFieldEvalNode::AND :
      setValue("&");
      break;
    case TableFieldEvalNode::OR :
      setValue("|");
      break;
    case TableFieldEvalNode::NOT :
      setValue("!");
      break;
    case TableFieldEvalNode::L_PAREN :
      setValue("(");
      break;
    case TableFieldEvalNode::R_PAREN :
      setValue(")");
      break;
    default :
      break;
    }
  }
}

