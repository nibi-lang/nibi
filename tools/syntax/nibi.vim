" Vim syntax file
" Language: Nibi
" Last Change: 2023
" Author: Josh Bosley
" Maintainer: Josh Bosley

if exists('b:current_syntax')
  finish
endif

:highlight CommandTags guifg=#87afaf
:highlight NibiComment guifg=#afffff
:highlight NibiDataList guifg=#8787ff
:highlight NibiAccessList guifg=red
:highlight NibiInstructionList guifg=#5fafd7

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

syn keyword nibiFunc set fn drop try throw assert alias
syn keyword nibiFunc env at iter eval quote loop exit quote import use macro
syn keyword nibiFunc int str float split type len clone nop dict
syn keyword nibiFunc i8 i16 i32 i64 u8 u16 u32 u64 f32 f64
syn keyword nibiQuickType true false nil nan inf

syn match nibiDictType ':let\|:get\|:del\|:keys\|:vals' contained
syn match nibiTagType ':u8\|:u16\|:u32\|:u64\|:i8\|:i16\|:i32\|:i64\|:f32\|:f64\|:int\|:float\|:ptr\|:str' contained

syn match nibiFunc '\(eq\|>\|<\|neq\|<=\|>=\|and\|or\|not\|if\|+\|-\|*\|/\)' contained
syn match nibiFunc '\(bw-and\|bw-or\|bw-xor\|bw-not\|bw-lsh\|bw-rsh\|<-\)' contained
syn match nibiFunc '\(extern-call\|mem-new\|mem-del\|mem-cpy\|mem-load\)' contained
syn match nibiFunc '\(mem-owned\|mem-acquire\|mem-abandon\|mem-is-set\)' contained
syn match nibiFunc ':=' contained
syn match nibiFunc '<<|' contained
syn match nibiFunc '|>>' contained
syn match nibiFunc "<|>" contained
syn match nibiFunc '>|' contained
syn match nibiFunc '|<' contained
syn match nibiFunc '|<' contained

hi def link nibiFunc Function
hi def link nibiQuickType Type
hi def link nibiNumber Number
hi def link nibiComment NibiComment
hi def link nibiIdentifier Identifier
hi def link nibiString String
hi def link nibiInsList NibiInstructionList
hi def link nibiDataList NibiDataList
hi def link nibiAccessList NibiAccessList
hi def link nibiKeyword Keyword
hi def link nibiTagType Type
hi def link nibiDictType CommandTags

let b:current_syntax = "nibi"

