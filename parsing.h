#ifndef PARSING_H
#define PARSING_H

#include <QString>
#include <QStringList>
#include <QListWidgetItem>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

enum ItemType {it_img, it_text, it_error, it_document, it_tag};

struct RegExp
{
    // general tags
    QString doctype = "^(<!DOCTYPE( +)HTML( +)PUBLIC( +)\"-\/\/W3C\/\/DTD HTML 3.2 Final\/\/EN\">)";
    QString html = "^(<html>(.*)<\/html>)$";
    QString head = "^(<head>(.*)<\/head>)";
    QString body = "^(<body>(.*)<\/body>)";

    // general
    QString link = "((https?:)?\/\/((([a-z0-9A-Z]*)\\.(\\w+))+)(.*))";
//    QString text = "[a-zA-Z0-9à-ÿÀ-ß_\- =+\/\\'\"!]*";
    QString text = "[^<>]*";
    QString href = "(href( *)=( *)\"(" + text + ")\")";
    QString attribute = "([\\w\\-]+)( *)=( *)\"[^<>\"]*\"";

    // for head tags
    QString head_title = "^(<title>(" + text + ")<\/title>)";
    QString head_base = "^(<base( +)" + href + ">)";
    QString head_meta = "^(<meta(( +)(content( *)=( *)\"(" + text + ")\"|http-equiv( *)=( *)\"(" + text + ")\"|name( *)=( *)\"(" + text + ")\"))*>)";
    QString head_isindex = "^(<isindex(( +)(" + href + "|prompt( *)=( *)\"(" + text + ")\")){0,1}>)";
    QString head_link = "^(<link(( +)(title( *)=( *)\"(" + text + ")\"|title( *)=( *)\"(Home|ToC|Index|Glossary|Copyright|Up|Next|Previous|Help|Bookmark)\"|rel( *)=( *)\"(" + text + ")\"))*>)";
    QString head_nextid = "^(<nextid( +)n=\"(Z|z)([0-9]{1,3})\">)";

    // for body tags
    QString body_text = "^([^(\<(.*)\>)]+)";
    QString body_br = "^(<br( *)\/?>)";
    QString body_hr = "^(<hr( *)\/?>)";
    QString body_p = "^(<p>(.*)<\/p>)";
};

struct Attribute
{
    QString name;
    QString value;
};

struct Item
{
    ItemType type;
    QString tag;
    QString data;
    QList<Item> children;
    QList<Attribute> attributes;
};

class Parsing
{
public:
    inline Parsing() {this->setData();}
    inline Parsing(QString data) {this->setData(data);}

    inline void setData(QString data = "") {this->_data = data; this->_offset = 0; this->title_first = ""; this->title_last = "";}

    inline const QString getTitleFirst()const {return this->title_first;}
    inline const QString getTitleLast()const {return this->title_last;}

    QList<QListWidgetItem*> parse();

private:
    QString title_first;
    QString title_last;
    QString _data;
    int _offset;
    RegExp _RegExp;

    QList<QListWidgetItem*> convert(Item);
    Item event(Item (Parsing::*fn)(Item), QString reg_exp, QString tag_name);
    QString attrs_to_string(QList<Attribute>);

    Item document();
    Item doctype(Item item);
    Item html(Item item);
    Item head(Item item);
    Item body(Item item);

    QList<Attribute> get_attributes(QString str);

    // head tags
    Item head_title(Item item);
    Item head_base(Item item);
    Item head_isindex(Item item);
    Item head_meta(Item item);
    Item head_link(Item item);
    Item head_nextid(Item item);

    // body tags
    Item body_p(Item item);
    Item body_text(Item item);
    Item body_br(Item item);
    Item body_hr(Item item);

    // main funcs
    void body_tag(Item &item);
};

#endif // PARSING_H
