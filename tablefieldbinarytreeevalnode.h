#ifndef BINARYTREEEVALNODE_H
#define BINARYTREEEVALNODE_H

#include <QObject>
#include <QList>
#include <QVector>
#include "tablefieldevalnode.h"

class QTextStream;

/**************************************************************************
 * \class TableFieldBinaryTreeEvalNode
 * \brief Wrap a TableFieldEvalNode into a tree with a parent and children.
 *
 * Despite the name, this need not be a binary tree. The children are traversed from left to right (first to last added).
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2011-2014
 ***************************************************************************/
class TableFieldBinaryTreeEvalNode : public QObject
{
    Q_OBJECT
public:
    explicit TableFieldBinaryTreeEvalNode(QObject *headNodeParent = nullptr);

    //virtual void clear();
    
    QTextStream& toStream(QTextStream& stream) const;

    //**************************************************************************
    /*! \brief Create a tree that can be evaluated.
     *
     *  Each BinaryTreeEvalNode contains an EvalNode.
     *  Each EvalNode in the returned list is owned by the BinaryTreeEvalNode that contains it.
     *  The head BinaryTreeEvalNode is "owned" by the headNodeParent.
     *  All other BinaryTreeEvalNode objects are owned by the head node, so, deleting the head node will
     *  delete the rest of the nodes.
     *
     *
     *  \param [in] list List of nodes to be turned into a parse tree.
     *  \param [in, out] headNodeParent QObject parent for the head node.
     *  \return A clone/copy of this object.
     ***************************************************************************/
    static TableFieldBinaryTreeEvalNode* buildTree(QList<TableFieldEvalNode *>& list, QObject *headNodeParent = nullptr);

    bool testEvaluator(const QHash<QString, bool>& vals) const;

    void addChild(TableFieldBinaryTreeEvalNode* child);

    void setNode(TableFieldEvalNode* node);
    void setTreeParent(TableFieldBinaryTreeEvalNode* node);

    /*! \brief Get the node type. */
    TableFieldEvalNode::OperatorType nodeType() const;

    /*! \brief Get the node value. */
    const QString nodeValue() const;

    /*! \brief Get the node priority. */
    int nodePriority() const;



    /*! \brief Shortcut to determine if the operator is infix  (a + b). */
    bool isInfix() const;

    /*! \brief Shortcut to determine if the operator is prefix (+ a). */
    bool isPrefix() const;

    /*! \brief Shortcut to determine if the operator is suffix (a b +). */
    bool isSuffix() const;

    /*! \brief Shortcut to determine if the operator is suffix or prefix. */
    bool isSuffixOrPrefix() const;

    /*! \brief Shortcut to determine if the operator has no mode. */
    bool hasNoMode() const;

    /*! \brief Shortcut to determine if the operator is NOT. */
    bool isNot() const;

    /*! \brief Shortcut to determine if the operator is AND. */
    bool isAnd() const;

    /*! \brief Shortcut to determine if the operator is OR. */
    bool isOr() const;

    /*! \brief Shortcut to determine if the operator is (. */
    bool isLeftParen() const;

    /*! \brief Shortcut to determine if the operator is ). */
    bool isRightParen() const;

    /*! \brief Shortcut to determine if the operator is a value against which to compare. */
    bool isValue() const;

    /*! \brief Shortcut to determine if the operator has no type (probably an error during evaluation will occur). */
    bool isNoType() const;

signals:
    
public slots:

private:
    //**************************************************************************
    /*! Copy constructor.
     *  \param [in] obj Object to copy.
     ***************************************************************************/
    TableFieldBinaryTreeEvalNode(const TableFieldBinaryTreeEvalNode&);

    //**************************************************************************
    /*! \brief Assignment operator.
     *
     *  \param [in] obj Object to copy.
     *  \return Reference to this object.
     ***************************************************************************/
    const TableFieldBinaryTreeEvalNode& operator=(const TableFieldBinaryTreeEvalNode&);

    //**************************************************************************
    /*! \brief Delete the parameter object if this object is the parent of the parameter object.
     *
     *  \param [in] obj Object to delete if "this" object is the parent of obj.
     *  \return True if the parameter object was deleted.
     ***************************************************************************/
    virtual bool deleteIfIAmParent(TableFieldEvalNode* obj) const;

    //**************************************************************************
    /*! \brief Pop the top operator and then any values as needed.
     *
     *  \param [in,out] values Stack of values against which to operate.
     *  \param [in,out] operators Stack of operators that are in process.
     *  \return True if this level evaluates to true, false otherwise.
     ***************************************************************************/
    static bool processOneTreeLevel(QStack<TableFieldBinaryTreeEvalNode*>& values, QStack<TableFieldBinaryTreeEvalNode*>& operators);

    /*! \brief Parent object. */
    TableFieldBinaryTreeEvalNode* m_treeParent;

    /*! \brief Node object. */
    TableFieldEvalNode* m_node;

    /*! \brief List of children, process first to last. */
    QVector<TableFieldBinaryTreeEvalNode*> m_children;
};

inline void TableFieldBinaryTreeEvalNode::setTreeParent(TableFieldBinaryTreeEvalNode* node)
{
    m_treeParent = node;
}

inline int TableFieldBinaryTreeEvalNode::nodePriority() const
{
    return m_node != nullptr ? m_node->nodePriority() : -1;
}

inline TableFieldEvalNode::OperatorType TableFieldBinaryTreeEvalNode::nodeType() const
{
    return m_node != nullptr ? m_node->nodeType() : TableFieldEvalNode::NO_TYPE;
}

inline const QString TableFieldBinaryTreeEvalNode::nodeValue() const
{
    return m_node != nullptr ? m_node->nodeValue() : "";
}

inline bool TableFieldBinaryTreeEvalNode::isNot() const
{
    return m_node != nullptr ? m_node->isNot() : false;
}

inline bool TableFieldBinaryTreeEvalNode::isAnd() const
{
    return m_node != nullptr ? m_node->isAnd() : false;
}

inline bool TableFieldBinaryTreeEvalNode::isOr() const
{
    return m_node != nullptr ? m_node->isOr() : false;
}

inline bool TableFieldBinaryTreeEvalNode::isLeftParen() const
{
    return m_node != nullptr ? m_node->isLeftParen() : false;
}

inline bool TableFieldBinaryTreeEvalNode::isRightParen() const
{
    return m_node != nullptr ? m_node->isRightParen() : false;
}

inline bool TableFieldBinaryTreeEvalNode::isValue() const
{
    return m_node != nullptr ? m_node->isValue() : false;
}

inline bool TableFieldBinaryTreeEvalNode::isNoType() const
{
    return m_node != nullptr ? m_node->isNoType() : true;
}

inline bool TableFieldBinaryTreeEvalNode::isSuffix() const
{
    return m_node != nullptr ? m_node->isSuffix() : false;
}

inline bool TableFieldBinaryTreeEvalNode::isPrefix() const
{
    return m_node != nullptr ? m_node->isPrefix() : false;
}

inline bool TableFieldBinaryTreeEvalNode::isInfix() const
{
    return m_node != nullptr ? m_node->isInfix() : false;
}

inline bool TableFieldBinaryTreeEvalNode::isSuffixOrPrefix() const
{
    return m_node != nullptr ? m_node->isSuffixOrPrefix() : false;
}

inline bool TableFieldBinaryTreeEvalNode::hasNoMode() const
{
    return m_node != nullptr ? m_node->hasNoMode() : true;
}



#endif // BINARYTREEEVALNODE_H
