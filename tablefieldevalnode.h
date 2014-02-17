#ifndef EVALNODE_H
#define EVALNODE_H

#include <QObject>

/**************************************************************************
 * \class EvalNode
 * \brief Represents an "evaluation node" for a set of AND / OR filters on a table.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2011-2014
 ***************************************************************************/

class TableFieldEvalNode : public QObject
{
    Q_OBJECT
    Q_ENUMS(OperatorType)
    //Q_ENUMS(OperatorMode)
public:

    enum OperatorType {NO_TYPE, VALUE, AND, OR, NOT, L_PAREN, R_PAREN};

    static TableFieldEvalNode* createAnd(QObject *headNodeParent = nullptr);
    static TableFieldEvalNode* createOr(QObject *headNodeParent = nullptr);
    static TableFieldEvalNode* createNot(QObject *headNodeParent = nullptr);
    static TableFieldEvalNode* createLeftParen(QObject *headNodeParent = nullptr);
    static TableFieldEvalNode* createRightParen(QObject *headNodeParent = nullptr);
    static TableFieldEvalNode* createValue(const QString& value, QObject *headNodeParent = nullptr);


    //**************************************************************************
    /*! Simple constructor.
     *  \param [in] parent Objects owner.
     ***************************************************************************/
    explicit TableFieldEvalNode(QObject *headNodeParent = nullptr);

    //**************************************************************************
    /*! Copy constructor.
     *  \param [in] obj Object to copy.
     *  \param [in] parent Objects owner.
     ***************************************************************************/
    explicit TableFieldEvalNode(const TableFieldEvalNode& obj, QObject *headNodeParent = nullptr);

    explicit TableFieldEvalNode(const OperatorType oType, QObject *headNodeParent = nullptr);

    explicit TableFieldEvalNode(const OperatorType oType, const QString& value, QObject *headNodeParent = nullptr);

    //**************************************************************************
    /*! Clone this object. You own it (or the owner does), so you must delete it.
     *  \param [in] parent New objects owner.
     *  \return A clone/copy of this object.
     ***************************************************************************/
    virtual TableFieldEvalNode* clone(QObject *headNodeParent = nullptr);

    //**************************************************************************
    /*! Get the priority of an operator.
     *  \param [in] oType Operator type.
     *  \return Priority of this type.
     ***************************************************************************/
    static int operatorPriority(const OperatorType oType);

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

    //**************************************************************************
    /*! \brief Assignment operator.
     *
     *  \param [in] obj Object to copy.
     *  \return Reference to this object.
     ***************************************************************************/
    const TableFieldEvalNode& operator=(const TableFieldEvalNode& obj);
    
    /*! \brief Get the node type. */
    OperatorType nodeType() const;

    /*! \brief Get the node value, which is a proxy for "name" that maps to a boolean value passed in later. */
    const QString& nodeValue() const;

    /*! \brief Get the node priority. */
    int nodePriority() const;

    //**************************************************************************
    /*! Set the node type.
     *
     *  \param [in] aType Node type.
     *  \param [in] fillRest If true, the node value is set to a nice value for pretty printing and the node priority is set.
     ***************************************************************************/
    void setType(const OperatorType aType, bool fillRest = false);

    //**************************************************************************
    /*! Set the node type.
     *
     *  \param [in] aType Node type.
     *  \param [in] aValue Set the node value.
     *  \param [in] fillRest If true, the node priority is set.
     ***************************************************************************/
    void setType(const OperatorType aType, const QString& aValue, bool fillRest = false);

    //**************************************************************************
    /*! Set the node value.
     *
     *  \param [in] aValue Set the node value.
     ***************************************************************************/
    void setValue(const QString& aValue);

    //**************************************************************************
    /*! Set the node priority.
     *
     *  \param [in] aPriority Set the node priority.
     ***************************************************************************/
    void setPriority(int aPriority);

signals:
    
public slots:
    
private:
    /*! \brief AND, OR, NOT, Value, parenthesis, etc.. */
    OperatorType m_type;

    /*! \brief Priority for this operator during evaluation. */
    int m_priority;

    /*! \brief In case a value is required. TODO: Should this be a variant? */
    QString m_value;
};

inline TableFieldEvalNode::OperatorType TableFieldEvalNode::nodeType() const
{
    return m_type;
}

inline const QString& TableFieldEvalNode::nodeValue() const
{
    return m_value;
}

inline int TableFieldEvalNode::nodePriority() const
{
    return m_priority;
}

inline void TableFieldEvalNode::setValue(const QString& aValue)
{
  m_value = aValue;
}

inline void TableFieldEvalNode::setPriority(int aPriority)
{
  m_priority = aPriority;
}

inline bool TableFieldEvalNode::isNot() const
{
    return nodeType() == TableFieldEvalNode::NOT;
}

inline bool TableFieldEvalNode::isAnd() const
{
    return nodeType() == TableFieldEvalNode::AND;
}

inline bool TableFieldEvalNode::isOr() const
{
    return nodeType() == TableFieldEvalNode::OR;
}

inline bool TableFieldEvalNode::isLeftParen() const
{
    return nodeType() == TableFieldEvalNode::L_PAREN;
}

inline bool TableFieldEvalNode::isRightParen() const
{
    return nodeType() == TableFieldEvalNode::R_PAREN;
}

inline bool TableFieldEvalNode::isValue() const
{
    return nodeType() == TableFieldEvalNode::VALUE;
}

inline bool TableFieldEvalNode::isNoType() const
{
    return nodeType() == TableFieldEvalNode::NO_TYPE;
}

inline bool TableFieldEvalNode::isSuffix() const
{
    return false;
}

inline bool TableFieldEvalNode::isPrefix() const
{
    return isNot();
}

inline bool TableFieldEvalNode::isInfix() const
{
    return isAnd() || isOr();
}

inline bool TableFieldEvalNode::isSuffixOrPrefix() const
{
    return isPrefix();
}

inline bool TableFieldEvalNode::hasNoMode() const
{
    return !(isInfix() || isSuffixOrPrefix());
}


#endif // EVALNODE_H
