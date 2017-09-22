#include "Yaml.h"
#include "FileUtils.h"
#include "Throw.h"
#include "StringEqual.h"

#include "libyaml/yaml.h"


#include <stdexcept>
#include <memory>
#include <cassert>
#include <tuple>

namespace Common
{
    struct safe_yaml_document {
        yaml_document_t doc;
        ~safe_yaml_document() {
            yaml_document_delete(&doc);
        }

        operator yaml_document_t* () {
            return &doc;
        }
    };

    using yaml_document_ptr = std::shared_ptr<safe_yaml_document>;

    struct YamlNode::Impl {
        yaml_document_ptr document;
        yaml_node_t *node = nullptr;

        static YamlNode CreateNode( const yaml_document_ptr &doc, yaml_node_t *node )
        {
            YamlNode ret;
            ret.mImpl->document = doc;
            ret.mImpl->node = node;
            return ret;
        }
    };

    COMMON_API YamlNode YamlNode::LoadFile( const char *filename )
    {
        std::string content = FileUtils::getFileContent(filename, false);
        return LoadString(content.c_str(), content.size());
    }

    COMMON_API YamlNode YamlNode::LoadFile( Archive &archive, const char *filename)
    {
        auto file = archive.openFile(filename);
        if (!file) return YamlNode();

        const char *content = (const char*)archive.mapFile(file);
        size_t size = archive.fileSize(file);

        return LoadString(content, size);
    }

    COMMON_API YamlNode YamlNode::LoadString( const char *str, size_t len )
    {
        yaml_parser_t parser;
        yaml_parser_initialize(&parser);
        yaml_parser_set_input_string(&parser, (const unsigned char*)str, len);
        yaml_parser_set_encoding(&parser, YAML_UTF8_ENCODING);

        yaml_document_ptr doc = std::make_shared<safe_yaml_document>();

        if (yaml_parser_load(&parser, *doc) != 1) {
            THROW(std::runtime_error, 
                  "(Failed to load yaml document!, error: \"%s\" at %zu:%zu", parser.problem ? parser.problem : "unknown", parser.problem_mark.line, parser.problem_mark.column
            );
        }

        yaml_node_t *node = yaml_document_get_root_node(*doc);
        return Impl::CreateNode(doc, node);
    }

    COMMON_API YamlNode::YamlNode()
    {
    }

    COMMON_API YamlNode::~YamlNode()
    {
    }

    COMMON_API YamlNode::YamlNode( const YamlNode& ) = default;
    COMMON_API YamlNode& YamlNode::operator = ( const YamlNode& ) = default;

    COMMON_API YamlNode::Type YamlNode::type() const
    {
        if (mImpl->node == nullptr) return Type::Null;

        switch (mImpl->node->type) {
        case YAML_NO_NODE:
            return Type::Null;
        case YAML_SCALAR_NODE:
            return  Type::Scalar;
        case YAML_SEQUENCE_NODE:
            return Type::Sequence;
        case YAML_MAPPING_NODE:
            return Type::Map;
        }
        return Type::Null;
    }

    COMMON_API size_t YamlNode::size() const
    {
        switch (type()) {
        case Type::Null:
        case Type::Scalar:
            return 0;
        case Type::Sequence:
            return mImpl->node->data.sequence.items.top - mImpl->node->data.sequence.items.start;
        case Type::Map:
            return mImpl->node->data.mapping.pairs.top - mImpl->node->data.mapping.pairs.start;
        }
        return 0;
    }

    COMMON_API YamlNode YamlNode::operator [] ( int i ) const
    {
        if (type() != Type::Sequence) return YamlNode();
        if (i < 0) return YamlNode();
        if (i >= size()) return YamlNode();

        const auto &sequence = mImpl->node->data.sequence;
        yaml_node_item_t item = sequence.items.start[i];

        yaml_node_t *node = yaml_document_get_node(*mImpl->document, item);
        return Impl::CreateNode(mImpl->document, node);
    }

    COMMON_API YamlNode YamlNode::operator[]( const char *str ) const
    {
        if (type() != Type::Map) return YamlNode();

        size_t len = strlen(str);

        const auto &mapping = mImpl->node->data.mapping;
        for (yaml_node_pair_t *iter=mapping.pairs.start; iter != mapping.pairs.top; ++iter) {
            yaml_node_t *key = yaml_document_get_node(*mImpl->document, iter->key);
            if (key == nullptr) continue;
            if (key->type != YAML_SCALAR_NODE) continue;

            if (StringUtils::equal((const char*)key->data.scalar.value, key->data.scalar.length, str, len)) {
                yaml_node_t *value = yaml_document_get_node(*mImpl->document, iter->value);
                return Impl::CreateNode(mImpl->document, value);
            }
        }

        return YamlNode();
    }

    COMMON_API std::string YamlNode::scalar() const
    {
        if (type() != Type::Scalar) return std::string();

        const auto &scalar = mImpl->node->data.scalar;

        return std::string((const char*)scalar.value, scalar.length);
    }

    COMMON_API const char* YamlNode::c_str( size_t &len ) const
    {
        if (type() != Type::Scalar) {
            len = 0;
            return nullptr;
        }

        const auto &scalar = mImpl->node->data.scalar;
        len = scalar.length;
        return (const char*)scalar.value;
    }

    COMMON_API StringId YamlNode::scalarAsStringId() const
    {
        if (type() != Type::Scalar) return StringId();

        const auto &scalar = mImpl->node->data.scalar;
        return CreateStringId((const char*)scalar.value, scalar.length);
    }
    
    COMMON_API YamlNode::Mark YamlNode::mark() const
    {
        if (mImpl->node) {
            Mark mark;
                mark.line = (int) mImpl->node->start_mark.line;
                mark.col = (int) mImpl->node->start_mark.column;
            return mark;
        }
        return Mark();
    }

    COMMON_API bool operator == ( const YamlNode &lhs, const YamlNode &rhs )
    {
        return lhs.mImpl->node == rhs.mImpl->node;
    }

    COMMON_API const char* YamlNode::_str() const
    {
        assert(isScalar());

        return (const char*)mImpl->node->data.scalar.value;
    }

    COMMON_API size_t YamlNode::_len() const
    {
        return mImpl->node->data.scalar.length;
    }

    struct YamlNode::const_iterator::Impl {
        YamlNode node;
        int idx = 0;
    };


    COMMON_API YamlNode::const_iterator YamlNode::begin() const
    {
        const_iterator iter;
        switch (type()) {
        case Type::Map:
        case Type::Sequence:
            iter.mImpl->node = *this;
            break;
        case Type::Scalar:
        case Type::Null:
            break;
        }
        return iter;
    }

    COMMON_API YamlNode::const_iterator YamlNode::end() const
    {
        const_iterator iter;
        switch (type()) {
        case Type::Map:
        case Type::Sequence:
            iter.mImpl->node = *this;
            iter.mImpl->idx = size();
            break;
        case Type::Scalar:
        case Type::Null:
            break;
        }
        return iter;
    }


    COMMON_API void YamlNode::const_iterator::advance( ptrdiff_t n )
    {
        mImpl->idx += n;
    }

    COMMON_API YamlNode::IteratorElement YamlNode::const_iterator::current() const
    {
        const auto &doc = mImpl->node.mImpl->document;

        switch(mImpl->node.type()) {
        case Type::Sequence: {
            auto seq = mImpl->node.mImpl->node->data.sequence;
            yaml_node_t *node = yaml_document_get_node(*doc, seq.items.start[mImpl->idx]);
            return YamlNode::Impl::CreateNode(doc, node);
        } case Type::Map: {
            auto map = mImpl->node.mImpl->node->data.mapping;
            auto entry = map.pairs.start[mImpl->idx];


            yaml_node_t *key = yaml_document_get_node(*doc, entry.key);
            yaml_node_t *value = yaml_document_get_node(*doc, entry.value);
            
            return std::make_pair(
                YamlNode::Impl::CreateNode(doc, key),
                YamlNode::Impl::CreateNode(doc, value)
            );
        }}

        return IteratorElement{};
    }

    YamlNode::const_iterator::const_iterator() = default;
    YamlNode::const_iterator::const_iterator( const const_iterator &) = default;
    YamlNode::const_iterator::~const_iterator() = default;

    COMMON_API YamlNode::const_iterator& YamlNode::const_iterator::operator = ( const const_iterator& ) = default;

    COMMON_API bool operator == ( const YamlNode::const_iterator &lhs, const YamlNode::const_iterator &rhs )
    {
        return lhs.mImpl->node == rhs.mImpl->node &&
               lhs.mImpl->idx == rhs.mImpl->idx;
    }
}