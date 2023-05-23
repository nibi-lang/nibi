" Vim syntax file
" Language: Nibi
" Last Change: 2023 May 22
" Author: Josh Bosley
" Maintainer: Josh Bosley

if exists('b:current_syntax')
  finish
endif

syn match nibiComment "#.*$"

syn region nibiInsList start='(' end=')' contains=nibiString,nibiNumber,nibiIdentifier,nibiOperator,nibiAssign,nibiListOperations,nibiKeywords,nibiRet,nibiAbsLists
syn match nibiString '".*"' contained 
syn match nibiIdentifier '[a-zA-Z_][a-zA-Z0-9_]*' contained
syn match nibiNumber '\d\+' contained
syn match nibiAssign ':=' contained 
syn match nibiOperator '\(eq\|>\|<\|neq\|<=\|>=\|and\|or\|not\|?\|+\|-\|*\|/\)' contained
syn match nibiListOperations '\(>|\||<\|<|>\)' contained
syn match nibiKeywords '\v(set|fn|drop|try|throw|assert|len|clone|put|putln|env|at|iter|eval|quote|loop|exit|quote|import|use)' contained 
syn match nibiRet '<-' contained
syn match nibiAbsLists '\({\|}\|\[\|]\)' contained

hi def link nibiNumber Number
hi def link nibiComment Comment
hi def link nibiIdentifier Identifier
hi def link nibiString String
hi def link nibiInsList Statement
hi def link nibiOperator Operator
hi def link nibiAssign Keyword
hi def link nibiListOperations Keyword
hi def link nibiKeywords Keyword
hi def link nibiRet Keyword
hi def link nibiAbsLists Keyword

let b:current_syntax = "nibi"

