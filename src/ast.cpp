#include "ast.h"

const TypeNode TypeNode::TYPE_U8 = TypeNode(Token(Token::Type::U8, 0, 0, "u8"));
const TypeNode TypeNode::TYPE_S8 = TypeNode(Token(Token::Type::S8, 0, 0, "s8"));
const TypeNode TypeNode::TYPE_U16 = TypeNode(Token(Token::Type::U16, 0, 0, "u16"));
const TypeNode TypeNode::TYPE_S16 = TypeNode(Token(Token::Type::S16, 0, 0, "s16"));
const TypeNode TypeNode::TYPE_U32 = TypeNode(Token(Token::Type::U32, 0, 0, "u32"));
const TypeNode TypeNode::TYPE_S32 = TypeNode(Token(Token::Type::S32, 0, 0, "s32"));
const TypeNode TypeNode::TYPE_U64 = TypeNode(Token(Token::Type::U64, 0, 0, "u64"));
const TypeNode TypeNode::TYPE_S64 = TypeNode(Token(Token::Type::S64, 0, 0, "s64"));
const TypeNode TypeNode::TYPE_USIZE = TypeNode(Token(Token::Type::USize, 0, 0, "usize"));
const TypeNode TypeNode::TYPE_SSIZE = TypeNode(Token(Token::Type::SSize, 0, 0, "ssize"));