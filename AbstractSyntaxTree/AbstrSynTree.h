#pragma once
/////////////////////////////////////////////////////////////////////
//  AbstrSynTree.h - Represents an Abstract Syntax Tree            //
//  ver 1.0                                                        //
//  Author:        Narendra Katamaneni, Syracuse University        //
/////////////////////////////////////////////////////////////////////
/*
  Package Operations:
  ==================
  This package defines an AbstrSynTree class and an ASTNode struct.  
  Those provides support for building Abstract Syntax Trees during 
  static source code analysis.

  Public Interface:
  =================
  AbstrSynTree ast(scopeStack);       // create instance, passing in ScopeStack
  ASTNode* pNode = ast.root();        // get or set root of AST
  ast.add(pNode);                     // add ASTNode to tree, linked to current scope
  ASTNode* pNode = ast.find(myType);  // retrieve ptr to ASTNode representing myType
  ast.pop();                          // close current scope by poping top of scopeStack

  Build Process:
  ==============
  Required files
  - ITokenCollection.h, ScopeStack.h

  Build commands (either one)
  - devenv CodeAnalysis.sln
  - cl /EHsc /DTEST_ABSTRSYNTREE AbstrSynTree.cpp Utilities.cpp /link setargv.obj

  Maintenance History:
  ====================
  ver 1.0 : 28 Feb 2019
  - first release

*/

#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <functional>
#include "../SemiExpression/ITokenCollection.h"
#include "../ScopeStack/ScopeStack.h"

namespace CodeAnalysis
{
  enum Access 
  { 
    publ, prot, priv 
  };

  enum DeclType 
  { 
    dataDecl, functionDecl, lambdaDecl, usingDecl 
  };

  struct DeclarationNode
  {
    Lexer::ITokenCollection* pTc = nullptr;
    Access access_;
    DeclType declType_;
    std::string package_;
    size_t line_;
  };

  struct ASTNode
  {
    using Type = std::string;
    using Name = std::string;
    using Package = std::string;
    using Path = std::string;

    ASTNode();
    ASTNode(const Type& type, const Name& name);
    ~ASTNode();
    Type type_;
    Type parentType_;
    Name name_;
    Package package_;
    Path path_;
    size_t startLineCount_;
    size_t endLineCount_;
    size_t complexity_;
    std::vector<ASTNode*> children_;
    std::vector<DeclarationNode> decl_;
    std::vector<Lexer::ITokenCollection*> statements_;
    std::string show(bool details = false);
  };

  class AbstrSynTree
  {
  public:
    using ClassName = std::string;
    using TypeMap = std::unordered_map<ClassName, ASTNode*>;

    AbstrSynTree(ScopeStack<ASTNode*>& stack);
    ~AbstrSynTree();
    ASTNode*& root();
    void add(ASTNode* pNode);
    ASTNode* find(const ClassName& type);
    ASTNode* pop();
    TypeMap& typeMap();
  private:
    TypeMap typeMap_;
    ScopeStack<ASTNode*>& stack_;
    ASTNode* pGlobalNamespace_;
  };
  //----< traverse AST and execute callobj on every node >-------------

  template <typename CallObj>
  void ASTWalk(ASTNode* pItem, CallObj co)
  {
    static size_t indentLevel = 0;
    co(pItem, indentLevel);
    auto iter = pItem->children_.begin();
    ++indentLevel;
    while (iter != pItem->children_.end())
    {
      ASTWalk(*iter, co);
      ++iter;
    }
    --indentLevel;
  }
  //----< traverse AST and execute callobj on every node >-------------

  template <typename CallObj>
  void ASTWalkNoIndent(ASTNode* pItem, CallObj co)
  {
    co(pItem);
    auto iter = pItem->children_.begin();
    while (iter != pItem->children_.end())
    {
      ASTWalkNoIndent(*iter, co);
      ++iter;
    }
  }
  //----< compute complexities for each ASTNode >--------------------

  inline void complexityWalk(ASTNode* pItem, size_t& count)
  {
    size_t inCount = ++count;
    auto iter = pItem->children_.begin();
    while (iter != pItem->children_.end())
    {
      complexityWalk(*iter, count);
      ++iter;
    }
    pItem->complexity_ = count - inCount + 1;
  }
  //----< compute complexities for each ASTNode >--------------------

  inline void complexityEval(ASTNode* pNode)
  {
    size_t initialCount = 0;
    complexityWalk(pNode, initialCount);
  }
}

struct foobar {

};
