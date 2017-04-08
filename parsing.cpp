#include "parsing.h"
#include <QDebug>

QList<QListWidgetItem*> Parsing::parse()
{
    _offset = 0;
    return this->convert(this->document());
}

QList<QListWidgetItem*> Parsing::convert(Item item)
{
    QList<QListWidgetItem*> result;
    switch(item.type)
    {
    case it_text:
        result.append(new QListWidgetItem(item.tag + "::" + item.data + attrs_to_string(item.attributes)));
        for(int i = 0; i < item.children.count(); i++)
            result.append(this->convert(item.children[i]));
        break;
    case it_error:
        result.append(new QListWidgetItem(item.data));
        break;
    case it_tag:
    case it_document:
        for(int i = 0; i < item.children.count(); i++)
            result.append(this->convert(item.children[i]));
        break;
    }
    if (item.tag == "title") {
        if (this->title_first == "")
            this->title_first = item.data;
        this->title_last = item.data;
    }
    return result;
}

QString Parsing::attrs_to_string(QList<Attribute> attrs)
{
    QString res("");
    for(int i = 0; i < attrs.count(); i++)
        res += " " + attrs[i].name + "=" + attrs[i].value;
    return res;
}

Item Parsing::document()
{
    Item document;
    document.data = "";
    document.type = it_document;
    document.children.append(event(doctype, _RegExp.doctype, "Doctype"));
    document.tag = "document";
    if (document.children[0].type != it_error) {
        document.children.append(event(html, _RegExp.html, "HTML"));
    }
    return document;
}

Item Parsing::event(Item(Parsing::*fn)(Item), QString reg_exp, QString tag_name)
{
    QRegularExpression re(reg_exp); // set Regular Expression
    QRegularExpressionMatch match = re.match(_data.mid(_offset)); // find data in current string in regular expression
    Item item;
    if (match.hasMatch()) {
        // if data found, we will call callback
        item.data = match.captured();
        item = (this->*fn)(item);
    } else {
        // if data with error, just create error message and exit
        item.data = tag_name + " not correct";
        item.type = it_error;
    }
    return item;
}

Item Parsing::doctype(Item item)
{
    item.type = it_tag;
    item.tag = "doctype";
    _offset = item.data.length();
    return item;
}

Item Parsing::html(Item item)
{
    item.type = it_tag;
    item.tag = "html";
    _offset += 6; // it is a length <html>
    item.children.append(event(head, _RegExp.head, "Head"));
    if (item.children[0].type != it_error)
        item.children.append(event(body, _RegExp.body, "Body"));
    return item;
}

Item Parsing::head(Item item)
{
    item.type = it_tag;
    item.tag = "head";
    int tag_size = _offset + item.data.length();
    _offset += 6; // it is a length <head>, after parsing it's content just increase offset to this length

    while(_offset < tag_size - 7){
        // we are inside <head> tag and don't stay at </head> (there are inside elements)
        // 7 - size of "</head>", we should stay at '<' after all events
        int old_offset = _offset;
        Item title = event(this->head_title, _RegExp.head_title, "Title");
        Item base = event(this->head_base, _RegExp.head_base, "Base");
        Item meta = event(this->head_meta, _RegExp.head_meta, "Meta");
        Item link = event(this->head_link, _RegExp.head_link, "Link");
        Item isindex = event(this->head_isindex, _RegExp.head_isindex, "Isindex");
        Item nextid = event(this->head_nextid, _RegExp.head_nextid, "Nextid");
        if (_offset == old_offset) {
            // nothing was happen, but we still stay at our position
            item.type = it_error;
            item.data = "Head includes some strange symbols";
            break;
        } else {
            if (title.type != it_error) item.children.append(title);
            if (base.type != it_error) item.children.append(base);
            if (meta.type != it_error) item.children.append(meta);
            if (link.type != it_error) item.children.append(link);
            if (isindex.type != it_error) item.children.append(isindex);
            if (nextid.type != it_error) item.children.append(nextid);
        }
    }

    _offset = tag_size; // -6 because we firstly increase to 6
    return item;
}

Item Parsing::body(Item item)
{
    item.tag = "body";
    item.type = it_text;
    int current_offset = _offset;
    _offset += 6; // it is a length <head>, after parsing it's content just increase offset to this length
    _offset = current_offset + item.data.length(); // -6 because we firstly increase to 6
    return item;
}

QList<Attribute> Parsing::get_attributes(QString str)
{
    QRegularExpression re(_RegExp.attribute);
    QRegularExpressionMatch match;
    QList<Attribute> attrs_res;
    bool been = true;
    while(been) {
        match = re.match(str); // find data in current string in regular expression
        if (match.hasMatch()) {
            // if data found, we will call callback
            QString attr_str = match.captured();
            // qDebug() << "Current attr in str: " + attr_str + " :: " + str;
            str = str.mid(match.capturedEnd());
            been = true;
            QStringList attrs_data = attr_str.split("=");
            if (attrs_data.count() == 2){
                Attribute attr;
                attr.name = attrs_data[0];
                attr.value = attrs_data[1];
                attrs_res.append(attr);
            }
        } else {
            been = false;
        }
    }
    return attrs_res;
}

// ============================================================================
// ============================================================================
// ============================================================================
// head tags
// ============================================================================
// ============================================================================
// ============================================================================
Item Parsing::head_title(Item item)
{
    item.type = it_tag;
    item.tag = "title";
    _offset += item.data.length();
    item.data = item.data.mid(7, item.data.length() - 15);
    return item;
}

Item Parsing::head_base(Item item)
{
    item.type = it_tag;
    item.tag = "base";
    _offset += item.data.length();
    item.attributes = get_attributes(item.data);
    item.data = "";
    return item;
}

Item Parsing::head_isindex(Item item)
{
    item.type = it_tag;
    item.tag = "isindex";
    _offset += item.data.length();
    item.attributes = get_attributes(item.data);
    item.data = "";
    return item;
}

Item Parsing::head_meta(Item item)
{
    item.type = it_tag;
    item.tag = "meta";
    _offset += item.data.length();
    item.attributes = get_attributes(item.data);
    item.data = "";
    return item;
}

Item Parsing::head_link(Item item)
{
    item.type = it_tag;
    item.tag = "link";
    _offset += item.data.length();
    item.attributes = get_attributes(item.data);
    item.data = "";
    return item;
}

Item Parsing::head_nextid(Item item)
{
    item.type = it_tag;
    item.tag = "nextid";
    _offset += item.data.length();
    item.attributes = get_attributes(item.data);
    item.data = "";
    return item;
}
