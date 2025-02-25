%{
#define TILDA 1
#define CMD1 2
#define CMD2 3
#define BEGINN 4
#define END 5
#define MATH 6
#define MATHDISPLAY 7
#define COMMENT 8
#define MATHMULTILINE 9
%}

space   [ \t\n]
spaces  {space}+
word   [a-zA-Z]
words \\{word}+
letter \\[^a-zA-Z]

beginenv \\begin{spaces}?\{[^{}]*\}
endenv   \\end{spaces}?\{[^{}]*\}
math \$[^$]*\$
mathline "\\["
mathline2 "\\]"
displaymath \${2}[^$]*\${2}

comment  %.*$

%%

{spaces}        { }
"~"             { return(TILDA); }
{mathline}      { return(MATHMULTILINE);}
{mathline2}      { return(MATHMULTILINE);}
{math}      { return(MATH); }
{displaymath}   { return(MATHDISPLAY); }
{beginenv}      { return(BEGINN); }
{endenv}        { return(END); }

{words}      { return(CMD1); }
{letter} { return(CMD2); }

{comment}       { /* Ignore comments */ }

.               { /* Ignore any other text */ }

%%
int yywrap(void) {
    return 1;
}
