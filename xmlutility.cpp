#include "xmlutility.h"
#include "globals.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

XMLUtility::XMLUtility()
{
}

QString XMLUtility::booleanToString(const bool b)
{
  return b ? "True" : "False";
}

bool XMLUtility::stringToBoolean(const QString& s)
{
  return s.compare("true", Qt::CaseInsensitive) == 0 || s.compare("t", Qt::CaseInsensitive) == 0 || s.compare("1", Qt::CaseSensitive) == 0 ? true : false;
}

QString XMLUtility::caseToString(Qt::CaseSensitivity sensitivity)
{
  return sensitivity == Qt::CaseInsensitive ? "CaseInsensitive" : "CaseSensitive";
}

Qt::CaseSensitivity XMLUtility::stringToCase(const QString& sensitivity)
{
  return sensitivity.contains("insensitive", Qt::CaseInsensitive) ? Qt::CaseInsensitive : Qt::CaseSensitive;
}


void XMLUtility::write(QXmlStreamWriter& writer, const QRegularExpression& regExp, const QString& name)
{
  if (regExp.pattern().isEmpty() || !regExp.isValid() || name.length() == 0)
  {
    return;
  }

  writer.writeStartElement(name);
  writer.writeAttribute("PatternOptions", QString::number((int) regExp.patternOptions()));
  //writer.writeAttribute("IsMinimal", booleanToString(regExp.isMinimal()));
  //writer.writeAttribute("CaseSensitive", caseToString(regExp.caseSensitivity()));
  //writer.writeAttribute("PatternSyntax", getEnumMapper().PatternSyntaxToString(regExp.patternSyntax()));
  writer.writeCharacters(regExp.pattern());
  writer.writeEndElement();
}

QRegularExpression* XMLUtility::readRegExp(QXmlStreamReader& reader)
{
  QRegularExpression* regExp = new QRegularExpression();
  if (reader.attributes().hasAttribute("PatternOptions"))
  {
    regExp->setPatternOptions((QRegularExpression::PatternOptions) reader.attributes().value("PatternOptions").toInt());
  }
  else if (reader.attributes().hasAttribute("CaseSensitive")) {
    QString cs = reader.attributes().value("CaseSensitive").toString();
    if (cs.compare("False", Qt::CaseInsensitive)) {
      regExp->setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    }
  }

  while (!reader.isEndElement())
  {
    if (reader.isCharacters())
    {
      regExp->setPattern(reader.text().toString());
    }
    reader.readNext();
  }
  if (!regExp->isValid())
  {
    delete regExp;
    regExp = nullptr;
  }

  return regExp;
}

void XMLUtility::writeElementAttribute(QXmlStreamWriter& writer, const QString& elementName, const QString& elementValue, const QString& attributeName, const QString& attributeValue)
{
  if (elementName.length() == 0)
  {
    return;
  }
  if (elementValue.length() > 0)
  {
    writer.writeStartElement(elementName);
  }
  else
  {
    writer.writeEmptyElement(elementName);
  }
  if (attributeName.length() > 0 && attributeValue.length() > 0)
  {
    writer.writeAttribute(attributeName, attributeValue);
  }

  if (elementValue.length() > 0)
  {
    writer.writeCharacters(elementValue);
    writer.writeEndElement();
  }
}

void XMLUtility::writeElement(QXmlStreamWriter& writer, const QString& elementName, const QString& elementValue)
{
  if (elementName.length() == 0)
  {
    return;
  }
  if (elementValue.length() > 0)
  {
    writer.writeStartElement(elementName);
    writer.writeCharacters(elementValue);
    writer.writeEndElement();
  }
  else
  {
    writer.writeEmptyElement(elementName);
  }
}
