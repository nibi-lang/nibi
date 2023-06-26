" Vim syntax file
" Language: Nibi
" Last Change: 2023 May 22
" Author: Josh Bosley
" Maintainer: Josh Bosley

if exists('b:current_syntax')
  finish
endif

syn cluster nibiCluster contains=ALL
syn region nibiString start=/"/ skip=/\\"/ end=/"/ oneline contains=nibiStringWrapper contained
syn region nibiStringWrapper start="\v\\\(\s*" end="\v\s*\)" contained containedin=nibiString contains=nibiInterpolatedString
syn region nibiInsList start='(' end=')' contains=@nibiCluster
syn region nibiDataList start='\[' end='\]' contained contains=@nibiCluster
syn region nibiAccessList start='{' end='}' contained contains=@nibiCluster

syntax match nibiInterpolatedString "\v\w+(\(\))?" contained containedin=nibiStringWrapper
syn match nibiComment "#.*$"
syn match nibiIdentifier '[a-zA-Z_][a-zA-Z0-9_]*' contained
syn match nibiNumber '\d\+' contained

syn keyword nibiFunc set fn drop try throw assert assert_eq assert_neq
syn keyword nibiFunc env at iter eval quote loop exit quote import use
syn keyword nibiFunc int str float split type len clone nop
syn keyword nibiQuickType true false nil nan inf

syn match nibiFunc '\(eq\|>\|<\|neq\|<=\|>=\|and\|or\|not\|if\|+\|-\|*\|/\)' contained
syn match nibiFunc '\(bw-and\|bw-or\|bw-xor\|bw-not\|bw-lsh\|bw-rsh\|<-\)' contained
syn match nibiFunc ':=' contained
syn match nibiFunc '<<|' contained
syn match nibiFunc '|>>' contained
syn match nibiFunc "<|>" contained
syn match nibiFunc '>|' contained
syn match nibiFunc '|<' contained
syn match nibiKeyword '|<' contained

hi def link nibiFunc Function
hi def link nibiQuickType Type
hi def link nibiNumber Number
hi def link nibiComment Comment
hi def link nibiIdentifier Identifier
hi def link nibiString String
hi def link nibiInsList Keyword
hi def link nibiDataList Keyword
hi def link nibiAccessList Keyword
hi def link nibiKeyword Keyword

let b:current_syntax = "nibi"

