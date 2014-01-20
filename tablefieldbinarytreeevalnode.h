#ifndef BINARYTREEEVALNODE_H
#define BINARYTREEEVALNODE_H

#include <QObject>
#include <QList>
#include <QVector>
#include "tablefieldevalnode.h"

class QTextStream;

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
    int nodePriority() const;

    TableFieldEvalNode::OperatorType nodeType() const;
    const QString nodeValue() const;

    bool isInfix() const;
    bool isPrefix() const;
    bool isSuffix() const;
    bool isSuffixOrPrefix() const;
    bool hasNoMode() const;

    bool isNot() const;
    bool isAnd() const;
    bool isOr() const;
    bool isLeftParen() const;
    bool isRightParen() const;
    bool isValue() const;
    bool isNoType() const;

signals:
    
public slots:

private:
    TableFieldBinaryTreeEvalNode(const TableFieldBinaryTreeEvalNode&);
    const TableFieldBinaryTreeEvalNode& operator=(const TableFieldBinaryTreeEvalNode&);

    virtual bool deleteIfIAmParent(TableFieldEvalNode* obj) const;
    static bool processOneTreeLevel(QStack<TableFieldBinaryTreeEvalNode*>& values, QStack<TableFieldBinaryTreeEvalNode*>& operators);

    TableFieldBinaryTreeEvalNode* m_treeParent;
    TableFieldEvalNode* m_node;
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
