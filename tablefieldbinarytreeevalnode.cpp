#include "tablefieldbinarytreeevalnode.h"

#include <QTextStream>
#include <QStack>

TableFieldBinaryTreeEvalNode::TableFieldBinaryTreeEvalNode(QObject *parent) :
    QObject(parent), m_treeParent(nullptr), m_node(nullptr)
{
}

void TableFieldBinaryTreeEvalNode::addChild(TableFieldBinaryTreeEvalNode* child)
{
    if (child != nullptr)
    {
        m_children.append(child);
        child->setTreeParent(this);
    }
}

void TableFieldBinaryTreeEvalNode::setNode(TableFieldEvalNode* node)
{
    deleteIfIAmParent(m_node);
    m_node = node;
}
#if 0
void BinaryTreeEvalNode::clear()
{
    // This node ONLY owns the node.
    deleteIfIAmParent(m_node);
    m_treeParent = nullptr;
    m_node = nullptr;
    m_children.clear(); //??
}
#endif
bool TableFieldBinaryTreeEvalNode::deleteIfIAmParent(TableFieldEvalNode* obj) const
{
    if (obj != nullptr && this == obj->parent())
    {
        delete obj;
        return true;
    }
    return false;
}

QTextStream& TableFieldBinaryTreeEvalNode::toStream(QTextStream& stream) const
{
    if (m_node == nullptr)
    {
      qDebug(qPrintable(tr("called toStream with a null node.")));
        return stream;
    }
    if (m_node->nodeType() == TableFieldEvalNode::VALUE)
    {
      stream << m_node->nodeValue();
      if (m_children.size() > 0)
      {
        qDebug(qPrintable(tr("Value element contains children!")));
      }
    }
    else if (m_node->nodeType() == TableFieldEvalNode::NO_TYPE || m_node->nodeType() == TableFieldEvalNode::R_PAREN || m_node->nodeType() == TableFieldEvalNode::L_PAREN)
    {
      qDebug(qPrintable(tr("Found a node of type NO_TYPE, L_PAREN, or R_PAREN.")));
    }
    else
    {
      if (m_node->isPrefix())
      {
        stream << " " << m_node->nodeValue() << " ";
      }

      if (m_children.size() > 0)
      {
        stream << "(";
        m_children.at(0)->toStream(stream);
        for (int i=1; i<m_children.size(); ++i)
        {
          if (m_node->isInfix())
          {
            stream << " " << m_node->nodeValue() << " ";
          }
          else
          {
            stream << ", ";
          }
          m_children.at(i)->toStream(stream);
        }
        stream << ")";
      }

      if (m_node->isSuffix())
      {
        stream << " " << m_node->nodeValue() << " ";
        qDebug(qPrintable(tr(" %1 ").arg(m_node->nodeValue())));
      }
    }
    return stream;
}

bool TableFieldBinaryTreeEvalNode::processOneTreeLevel(QStack<TableFieldBinaryTreeEvalNode*>& values, QStack<TableFieldBinaryTreeEvalNode*>& operators)
{
  if (operators.isEmpty())
  {
    qDebug(qPrintable(tr("Operator stack is prematurely empty")));
    return true;
  }
  TableFieldBinaryTreeEvalNode* opNode = operators.pop();
  //qDebug(qPrintable(tr("processOneTreeLevel Op Node value(%1) type(%2) priority(%3)").arg(opNode->nodeValue()).arg(opNode->nodeType()).arg(opNode->nodePriority())));
  if (opNode->m_node->isInfix())
  {
    // Two operands
    if (values.size() < 2)
    {
        qDebug(qPrintable(tr("Found an INFIX operator that requires two values, and have %1 value(s).").arg(values.size())));
        return true;
    }
    else
    {
        TableFieldBinaryTreeEvalNode* child2 = values.pop();
        TableFieldBinaryTreeEvalNode* child1 = values.pop();
        if (child1->nodeType() == opNode->nodeType())
        {
            // First child has the same operator type,
            // So, we can use a single node (faster processing and fewer nodes).
            if (child2->nodeType() == opNode->nodeType())
            {
                // So does child 2, so copy the children.
                for (int i=0; i<child2->m_children.size(); ++i)
                {
                    child1->addChild(child2->m_children.at(i));
                }
                delete child2;
            }
            else
            {
                child1->addChild(child2);
                values.push(child1);
            }
            delete opNode;
        }
        else if (child2->nodeType() == opNode->nodeType())
        {
            // Make child1 the first child of child two.
            child2->m_children.insert(0, child1);
            child1->setTreeParent(child2);
            delete opNode;
        }
        else
        {
            opNode->addChild(child1);
            opNode->addChild(child2);
            values.push(opNode);
        }
    }
  }
  else if (opNode->m_node->isSuffixOrPrefix())
  {
      // Need one value.
      if (values.isEmpty())
      {
          qDebug(qPrintable(tr("Found a PREFIX or SUFFIX operator with an empty value list.")));
          return true;
      }
      else
      {
          TableFieldBinaryTreeEvalNode* child1 = values.pop();
          // I suppose if the value is boolean...
          if (child1->isValue())
          {
            qDebug(qPrintable(tr("Cannot take the NOT of a value.")));
            return true;
          }
          // make the value a child of the NOT operator and push the NOT operator
          // into the values stack.
          opNode->addChild(child1);
          values.push(opNode);
      }
  }
  else
  {
      qDebug(qPrintable(tr("Found an operator node with an unknown type.")));
      return true;
  }
  return false;
}

TableFieldBinaryTreeEvalNode* TableFieldBinaryTreeEvalNode::buildTree(QList<TableFieldEvalNode *>& list, QObject *parent)
{
  if (list.isEmpty())
  {
      return nullptr;
  }

  // Build a new list that is surrounded with parenthesis.
  QList<TableFieldEvalNode *> safeList;
  TableFieldEvalNode leftParenEvalNode(TableFieldEvalNode::L_PAREN);
  TableFieldEvalNode rightParenEvalNode(TableFieldEvalNode::R_PAREN);
  TableFieldBinaryTreeEvalNode* headNode = new TableFieldBinaryTreeEvalNode(parent);

  safeList.append(&leftParenEvalNode);
  safeList.append(list);
  safeList.append(&rightParenEvalNode);

  bool isError = false;
  QStack<TableFieldBinaryTreeEvalNode*> values;
  QStack<TableFieldBinaryTreeEvalNode*> operators;

  QListIterator<TableFieldEvalNode*> i(safeList);
  while (i.hasNext() && !isError)
  {
      TableFieldEvalNode* eNode = i.next();
      if (eNode == nullptr)
      {
        qDebug(qPrintable(tr("Cannot build a tree from null nodes.")));
        isError = true;
      }
      else if (eNode->isValue())
      {
          // Push value nodes on the value stack.
          TableFieldBinaryTreeEvalNode* bNode = new TableFieldBinaryTreeEvalNode(headNode);
          bNode->setNode(eNode->clone(bNode));
          values.push(bNode);
      }
      else if (eNode->isLeftParen())
      {
          // Push left parenthesis onto the operator stack.
          // Will be removed when a right parenthesis is found.
          TableFieldBinaryTreeEvalNode* bNode = new TableFieldBinaryTreeEvalNode(headNode);
          bNode->setNode(eNode->clone(bNode));
          operators.push(bNode);
      }
      else if (eNode->isRightParen())
      {
          // Found a right parenthesis.
          // Process all operators until a left parenthesis is found then get rid of it.
          while (!isError && !operators.isEmpty() && !operators.top()->isLeftParen())
        {
          isError = processOneTreeLevel(values, operators);
        }
        if (!isError)
        {
          if (operators.isEmpty())
          {
            qDebug(qPrintable(tr("Have a ')' without a '('")));
            isError = true;
          }
          else
          {
            // Dispose of the open parenthesis.
            TableFieldBinaryTreeEvalNode* opNode = operators.pop();
            delete opNode;
          }
        }
      }
      else if (eNode->isNoType())
      {
          qDebug(qPrintable(tr("Cannot build a tree from raw nodes with no type")));
          isError = true;
      }
      else
      {
          // Found an operator.
          // Process all operators whose priority / precidence is the same or greater than this one.
          // So, after reading "a OR b OR", you know you can set "a OR b", but you don't know yet if you can
          // process the second OR.
        while (!isError && !operators.isEmpty() && !operators.top()->isLeftParen() && operators.top()->nodePriority() >= eNode->nodePriority())
        {
            isError = processOneTreeLevel(values, operators);
        }
        if (!isError)
        {
            // No error, so push the operator onto the operator stack.
            TableFieldBinaryTreeEvalNode* bNode = new TableFieldBinaryTreeEvalNode(headNode);
            bNode->setNode(eNode->clone(bNode));
            operators.push(bNode);
        }
      }
  }

  if (!isError)
  {
    if (!operators.isEmpty())
    {
      isError = true;
      qDebug(qPrintable(tr("Unprocessed Operators left in the stack after processing")));
    }
    else if (values.size() != 1)
    {
      isError = true;
      qDebug(qPrintable(tr("There should be only one value left after processing, not %1").arg(values.size())));
    }
    else
    {
        // tree parent node should already be null.
        // Perform a shallow copy of the top node into the head node.
        TableFieldBinaryTreeEvalNode* topNode = values.top();

        // Clone the node so that the parent / owner is properly set.
        headNode->m_node = topNode->m_node->clone(headNode);
        for (int i=0; i<topNode->m_children.size(); ++i)
        {
            headNode->addChild(topNode->m_children.at(i));
        }

        delete topNode;
        return headNode;
    }
  }
  if (isError)
  {
      // All of the created nodes are a child of the head node.
      delete headNode;
  }
  return nullptr;
}

bool TableFieldBinaryTreeEvalNode::testEvaluator(const QHash<QString, bool>& vals) const
{
    //qDebug(qPrintable(QString("Type(%1) Value(%2)").arg(nodeType()).arg(nodeValue())));
    if (m_node == nullptr)
    {
        qDebug("NULL Node in the evaluator");
    }
    else if (isValue())
    {
        //qDebug(qPrintable(QString("Returning %1").arg(vals.value(nodeValue()))));
        return vals.value(nodeValue());
    }
    else if (m_children.size() > 0)
    {
        bool rc = m_children.at(0)->testEvaluator(vals);
        if (isAnd())
        {
            for (int i=1; rc && i<m_children.size(); ++i)
            {
                rc = m_children.at(i)->testEvaluator(vals);
            }
        }
        else if (isOr())
        {
            for (int i=1; !rc && i<m_children.size(); ++i)
            {
                rc = m_children.at(i)->testEvaluator(vals);
            }
        }
        else if (isNot())
        {
            return !rc;
        }
        else
        {
            return false;
            qDebug("Wrong number of arguments in the evaluator.");
        }
        return rc;
    }
    else
    {
        qDebug("No children for the evaluator to evaluate");
    }
    return false;
}
