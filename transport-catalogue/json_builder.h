#pragma once
 
#include "json.h"
#include <stack>
#include <string>
#include <memory>

namespace transport_catalogue {
    namespace details {
        namespace json {

            class KeyContext;
            class DictionaryContext;
            class ArrayContext;

            class Builder {
            public:

                KeyContext Key(std::string key_);
                DictionaryContext StartDict();
                ArrayContext StartArray();

                Builder& Value(Node::Value value);

                Builder& EndDict();

                Builder& EndArray();

                Node Build();

            private:
                Node root_;
                std::vector<std::unique_ptr<Node>> nodes_stack_;

                void AddNode(Node node);
            };

            class BaseContext {
            public:
                BaseContext(Builder& builder);

                KeyContext Key(std::string key);
                DictionaryContext StartDict();
                ArrayContext StartArray();

                Builder& Value(Node::Value value);

                Builder& EndDict();

                Builder& EndArray();

            protected:
                Builder& builder_;

            };

            class KeyContext : public BaseContext {
            public:
                KeyContext(Builder& builder);

                KeyContext Key(std::string key) = delete;

                BaseContext EndDict() = delete;
                BaseContext EndArray() = delete;

                DictionaryContext Value(Node::Value value);
            };

            class DictionaryContext : public BaseContext {
            public:
                DictionaryContext(Builder& builder);

                DictionaryContext StartDict() = delete;

                ArrayContext StartArray() = delete;
                Builder& EndArray() = delete;

                Builder& Value(Node::Value value) = delete;
            };

            class ArrayContext : public BaseContext {
            public:
                ArrayContext(Builder& builder);

                KeyContext Key(std::string key) = delete;

                Builder& EndDict() = delete;

                ArrayContext Value(Node::Value value);
            };

        }
    }
}