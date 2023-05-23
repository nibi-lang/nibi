" Vim syntax file
" Language: Nibi
" Last Change: 2023 May 22
" Author: Josh Bosley
" Maintainer: Josh Bosley

if exists('b:current_syntax')
  finish
endif

syn match nibiComment "#.*$"

syn match nibiString '".*"'  
syn match nibiIdentifier '[a-zA-Z_][a-zA-Z0-9_]*' 
syn match nibiNumber '\d\+' 
syn match nibiAssign ':='  
syn match nibiOperator '\(eq\|>\|<\|neq\|<=\|>=\|and\|or\|not\|?\|+\|-\|*\|/\)' 
syn match nibiListOperations '\(>|\||<\|<|>\)' 
syn match nibiKeywords '\v(set|fn|drop|try|throw|assert|len|clone|put|putln|env|at|iter|eval|quote|loop|exit|quote|import|use)'  
syn match nibiRet '<-' 
syn match nibiAbsLists '\((\|)\|{\|}\|\[\|]\)' 

hi def link nibiNumber Number
hi def link nibiComment Comment
hi def link nibiIdentifier Identifier
hi def link nibiString String
hi def link nibiOperator Operator
hi def link nibiAssign Keyword
hi def link nibiListOperations Keyword
hi def link nibiKeywords Keyword
hi def link nibiRet Keyword
hi def link nibiAbsLists Keyword

let b:current_syntax = "nibi"

