/*
 * prabsyn.c - Print Abstract Syntax data structures. Most functions 
 *           handle an instance of an abstract syntax rule.
 */

#include <stdio.h>
#include "util.h"
#include "symbol.h" /* symbol table data structures */
#include "absyn.h"  /* abstract syntax data structures */
#include "prabsyn.h" /* function prototype */

/* local function prototypes */
static void pr_var(FILE *out, A_var v, int d);
static void pr_dec(FILE *out, A_dec v, int d);
static void pr_ty(FILE *out, A_ty v, int d);
static void pr_field(FILE *out, A_field v, int d);
static void pr_fieldList(FILE *out, A_fieldList v, int d);
static void pr_expList(FILE *out, A_expList v, int d);
static void pr_fundec(FILE *out, A_fundec v, int d);
static void pr_fundecList(FILE *out, A_fundecList v, int d);
static void pr_decList(FILE *out, A_decList v, int d);
static void pr_namety(FILE *out, A_namety v, int d);
static void pr_nametyList(FILE *out, A_nametyList v, int d);
static void pr_efield(FILE *out, A_efield v, int d);
static void pr_efieldList(FILE *out, A_efieldList v, int d);

static void indent(FILE *out, int d) {
 int i;
 for (i = 0; i <= d; i++) fprintf(out, " ");
}

/* Print A_var types. Indent d spaces. */
static void pr_var(FILE *out, A_var v, int d) {
 indent(out, d);
 switch (v->kind) {
 case A_simpleVar:
   fprintf(out, "simpleVar(%s)", S_name(v->u.simple)); 
   break;
 case A_fieldVar:
   fprintf(out, "%s\n", "fieldVar(");
   pr_var(out, v->u.field.var, d+1); fprintf(out, "%s\n", ","); 
   indent(out, d+1); fprintf(out, "%s)", S_name(v->u.field.sym));
   break;
 case A_subscriptVar:
   fprintf(out, "%s\n", "subscriptVar(");
   pr_var(out, v->u.subscript.var, d+1); fprintf(out, "%s\n", ","); 
   pr_exp(out, v->u.subscript.exp, d+1); fprintf(out, "%s", ")");
   break;
 default:
   assert(0); 
 } 
}

static char str_oper[][12] = {
   "PLUS", "MINUS", "TIMES", "DIVIDE", 
   "EQUAL", "NOTEQUAL", "LESSTHAN", "LESSEQ", "GREAT", "GREATEQ", 
   "BINAND", "MOD", "REM", "TICK", "AND", "OR",
   "XOR", "IN", "NOTIN", "RANGE", "DOT", "EXPON"};
 
static void pr_oper(FILE *out, A_oper d) {
  fprintf(out, "%s", str_oper[d]);
}

static char str_unoper[][12] = {
    "NOT", "ABS" , "UNARYPLUS", "UNARYMINUS"};

static void pr_unoper(FILE *out, A_unaryOper d) {
  fprintf(out, "%s", str_unoper[d]);
}

/* Print A_var types. Indent d spaces. */
void pr_exp(FILE *out, A_exp v, int d) {
 indent(out, d);
 switch (v->kind) {
 case A_varExp:
   fprintf(out, "varExp(\n"); pr_var(out, v->u.var, d+1); 
   fprintf(out, "%s", ")");
   break;
 case A_nilExp:
   fprintf(out, "nilExp()");
   break;
 case A_numberExp:
   fprintf(out, "numberExp(%s)", v->u.number);
   break;
 case A_stringExp:
   fprintf(out, "stringExp(%s)", v->u.stringg);
   break;
 case A_callExp:
   fprintf(out, "callExp(%s,\n", S_name(v->u.call.func));
   pr_expList(out, v->u.call.args, d+1); fprintf(out, ")");
   break;
 case A_opExp:
   fprintf(out, "opExp(\n"); pr_exp(out, v->u.op.left, d+1); 
   pr_oper(out, v->u.op.oper); fprintf(out, ",\n"); 
   pr_exp(out, v->u.op.right, d+1); fprintf(out, ")");
   break;
 case A_unaryOpExp:
   fprintf(out, "unaryOpExp(\n");
   indent(out, d+1); pr_unoper(out, v->u.unaryOp.oper); fprintf(out, ",\n"); 
   pr_exp(out, v->u.unaryOp.exp, d+1); fprintf(out, ")");
   break;
 case A_recordExp:
   fprintf(out, "recordExp(%s,\n", S_name(v->u.record.typ)); 
   pr_efieldList(out, v->u.record.fields, d+1); fprintf(out, ")");
   break;
 case A_seqExp:
   fprintf(out, "seqExp(\n");
   pr_expList(out, v->u.seq, d+1); fprintf(out, ")");
   break;
 case A_assignExp:
   fprintf(out, "assignExp(\n");
   pr_exp(out, v->u.assign.name, d+1); fprintf(out, ",\n");
   pr_exp(out, v->u.assign.exp, d+1); fprintf(out, ")");
   break;
 case A_ifExp:
   fprintf(out, "iffExp(\n");
   pr_expList(out, v->u.iff.cond_clauses, d+1); fprintf(out, ",\n");
   pr_expList(out, v->u.iff.elsee, d+1);
   fprintf(out, ")");
   break;
 case A_whileExp:
   fprintf(out, "whileExp(\n");
   pr_exp(out, v->u.whilee.test, d+1); fprintf(out, ")\n");
   break;
 case A_forExp:
   fprintf(out, "forExp(\n");
   pr_exp(out, v->u.forr.var, d+1);fprintf(out, ",\n");
   pr_exp(out, v->u.forr.reverse, d+1); fprintf(out, ",\n"); 
   pr_exp(out, v->u.forr.range, d+1); fprintf(out, ")\n");
   break;
 case A_loopExp:
   fprintf(out, "loopExp(\n");
   pr_exp(out, v->u.loop.labelopt, d+1); fprintf(out, ",\n");
   pr_exp(out, v->u.loop.iteration, d+1); fprintf(out, ",\n");
   pr_expList(out, v->u.loop.basicloop, d+1); fprintf(out, ",\n");
   pr_exp(out, v->u.loop.idopt, d+1); fprintf(out, ")\n");
   break;
 case A_condExp:
   fprintf(out, "condExp(\n");
   pr_exp(out, v->u.cond.test, d+1); fprintf(out, ",\n");
   pr_expList(out, v->u.cond.stmts, d+1); fprintf(out, ")\n");
   break;
 case A_breakExp:
   fprintf(out, "breakExp()");
   break;
 case A_exitExp:
   fprintf(out, "exitExp(\n");
   pr_exp(out, v->u.exit.exitname, d+1); fprintf(out, ",\n");
   pr_exp(out, v->u.exit.exitcondition, d+1); fprintf(out, ")\n");
   break;
 case A_letExp:
   fprintf(out, "letExp(\n");
   pr_decList(out, v->u.let.decs, d+1); fprintf(out, ",\n");
   pr_exp(out, v->u.let.body, d+1); fprintf(out, ")");
   break;
 case A_arrayExp:
   fprintf(out, "arrayExp(%s,\n", S_name(v->u.array.typ));
   pr_exp(out, v->u.array.size, d+1); fprintf(out, ",\n");
   pr_exp(out, v->u.array.init, d+1); fprintf(out, ")");
   break;
 case A_returnExp:
   fprintf(out, "returnExp(\n");
   pr_exp(out, v->u.retval, d+1); fprintf(out, ")\n");
   break;
 case A_gotoExp:
   fprintf(out, "gotoExp(\n");
   pr_exp(out, v->u.gotolabel, d+1); fprintf(out, ")\n");
   break;
 case A_pragma:
   fprintf(out, "pragma(\n");
   indent(out, d+1); fprintf(out,"%s", v->u.pragmaname); fprintf(out, ")\n");
   break;
 case A_pragmalist:
   fprintf(out, "pragmalist(\n");
   pr_exp(out, v->u.pragmalist.name, d+1); fprintf(out, ",\n");
   pr_expList(out, v->u.pragmalist.pragmaargs, d+1); fprintf(out, ")\n");
   break; 
 case A_alternative:
   fprintf(out, "alternative(\n");
   pr_expList(out, v->u.alternative.choices, d+1); fprintf(out, ",\n");
   pr_expList(out, v->u.alternative.stmts, d+1); fprintf(out, ")\n");
   break;
 case A_caseExp:
   fprintf(out, "case(\n");
   pr_exp(out, v->u.caseexp.header, d+1); fprintf(out, ",\n");
   pr_expList(out, v->u.caseexp.pragmas, d+1); fprintf(out, ",\n");
   pr_expList(out, v->u.caseexp.alternatives, d+1); fprintf(out, ")\n");
   break;
 case A_raiseExp:
   fprintf(out, "raiseExp(\n");
   pr_exp(out, v->u.nameopt, d+1); fprintf(out, ")\n");
   break;
 case A_procedure:
   fprintf(out, "procedure(\n");
   pr_exp(out, v->u.procedurename, d+1); fprintf(out, ")\n");
   break;
 case A_functionUse:
   fprintf(out, "functionUse(\n");
   pr_exp(out, v->u.functionuse.name, d+1); fprintf(out, ",\n");
   pr_expList(out, v->u.functionuse.value, d+1); fprintf(out, ")\n");
   break;
 case A_compAssoc:
   fprintf(out, "compAssoc(\n");
   pr_expList(out, v->u.compassoc.choices, d+1); fprintf(out, ",\n");
   pr_exp(out, v->u.compassoc.expression, d+1); fprintf(out, ")\n");
   break;
 case A_compUnit:
   fprintf(out, "compUnit(\n");
   pr_expList(out, v->u.compunit.contextspec, d+1); fprintf(out, ",\n");
   pr_exp(out, v->u.compunit.privatee, d+1); fprintf(out, ",\n");
   pr_exp(out, v->u.compunit.unit, d+1); fprintf(out, ",\n");
   pr_expList(out, v->u.compunit.pragmas, d+1); fprintf(out, ")\n");
   break;
 case A_useclause:
   fprintf(out, "useclause(\n");
   pr_exp(out, v->u.useclause.type, d+1); fprintf(out, ",\n");
   pr_expList(out, v->u.useclause.names, d+1); fprintf(out, ")\n");
   break;
 case A_contextSpecwith:
   fprintf(out, "contextspecwith(\n");
   pr_expList(out, v->u.contextspecwith.withclause, d+1); fprintf(out, ",\n");
   pr_expList(out, v->u.contextspecwith.useclauseopt, d+1); fprintf(out, ")\n");
   break;
 case A_subprogSpec:
   fprintf(out, "subprogspec(\n");
   pr_exp(out, v->u.subprogSpec.name, d+1); fprintf(out, ",\n");
   pr_expList(out, v->u.subprogSpec.formalpart, d+1); fprintf(out, ")\n");
   break;
 case A_notImplemented:
   fprintf(out, "******* %s ******", v->u.msg);
   break;
 default:
   assert(0); 
 } 
}

static void pr_dec(FILE *out, A_dec v, int d) {
 indent(out, d);
 switch (v->kind) {
 case A_functionDec:
   fprintf(out, "functionDec(\n"); 
   pr_fundecList(out, v->u.function, d+1); fprintf(out, ")");
   break;
 case A_varDec:
   fprintf(out, "varDec(%s,\n", S_name(v->u.var.var));
   if (v->u.var.typ) {
     indent(out, d+1); fprintf(out, "%s,\n", S_name(v->u.var.typ)); 
   }
   pr_exp(out, v->u.var.init, d+1); fprintf(out, ",\n");
   indent(out, d+1); fprintf(out, "%s", v->u.var.escape ? "TRUE)" : "FALSE)");
   break;
 case A_typeDec:
   fprintf(out, "typeDec(\n"); 
   pr_nametyList(out, v->u.type, d+1); fprintf(out, ")");
   break;
 default:
   assert(0); 
 } 
}

static void pr_ty(FILE *out, A_ty v, int d) {
 indent(out, d);
 switch (v->kind) {
 case A_nameTy:
   fprintf(out, "nameTy(%s)", S_name(v->u.name));
   break;
 case A_recordTy:
   fprintf(out, "recordTy(\n");
   pr_fieldList(out, v->u.record, d+1); fprintf(out, ")");
   break;
 case A_arrayTy:
   fprintf(out, "arrayTy(%s)", S_name(v->u.array));
   break;
 default:
   assert(0); 
 } 
}

static void pr_field(FILE *out, A_field v, int d) {
 indent(out, d);
 fprintf(out, "field(%s,\n", S_name(v->name));
 indent(out, d+1); fprintf(out, "%s,\n", S_name(v->typ));
 indent(out, d+1); fprintf(out, "%s", v->escape ? "TRUE)" : "FALSE)");
}

static void pr_fieldList(FILE *out, A_fieldList v, int d) {
 indent(out, d);
 if (v) {
   fprintf(out, "fieldList(\n");
   pr_field(out, v->head, d+1); fprintf(out, ",\n");
   pr_fieldList(out, v->tail, d+1); fprintf(out, ")");
 }
 else fprintf(out, "fieldList()");
}

static void pr_expList(FILE *out, A_expList v, int d) {
 indent(out, d);
 if (v) {
   fprintf(out, "expList(\n"); 
   pr_exp(out, v->head, d+1); fprintf(out, ",\n");
   pr_expList(out, v->tail, d+1);
   fprintf(out, ")");
 }
 else fprintf(out, "expList()"); 

}

static void pr_fundec(FILE *out, A_fundec v, int d) {
 indent(out, d);
 fprintf(out, "fundec(%s,\n", S_name(v->name));
 pr_fieldList(out, v->params, d+1); fprintf(out, ",\n");
 if (v->result) {
   indent(out, d+1); fprintf(out, "%s,\n", S_name(v->result));
 }
 pr_exp(out, v->body, d+1); fprintf(out, ")");
}

static void pr_fundecList(FILE *out, A_fundecList v, int d) {
 indent(out, d);
 if (v) {
   fprintf(out, "fundecList(\n"); 
   pr_fundec(out, v->head, d+1); fprintf(out, ",\n");
   pr_fundecList(out, v->tail, d+1); fprintf(out, ")");
 }
 else fprintf(out, "fundecList()");
}

static void pr_decList(FILE *out, A_decList v, int d) {
 indent(out, d);
 if (v) {
   fprintf(out, "decList(\n"); 
   pr_dec(out, v->head, d+1); fprintf(out, ",\n");
   pr_decList(out, v->tail, d+1);
   fprintf(out, ")");
 }
 else fprintf(out, "decList()"); 

}

static void pr_namety(FILE *out, A_namety v, int d) {
 indent(out, d);
 fprintf(out, "namety(%s,\n", S_name(v->name)); 
 pr_ty(out, v->ty, d+1); fprintf(out, ")");
}

static void pr_nametyList(FILE *out, A_nametyList v, int d) {
 indent(out, d);
 if (v) {
   fprintf(out, "nametyList(\n"); 
   pr_namety(out, v->head, d+1); fprintf(out, ",\n");
   pr_nametyList(out, v->tail, d+1); fprintf(out, ")");
 }
 else fprintf(out, "nametyList()");
}

static void pr_efield(FILE *out, A_efield v, int d) {
 indent(out, d);
 if (v) {
   fprintf(out, "efield(%s,\n", S_name(v->name));
   pr_exp(out, v->exp, d+1); fprintf(out, ")");
 }
 else fprintf(out, "efield()");
}

static void pr_efieldList(FILE *out, A_efieldList v, int d) {
 indent(out, d);
 if (v) {
   fprintf(out, "efieldList(\n"); 
   pr_efield(out, v->head, d+1); fprintf(out, ",\n");
   pr_efieldList(out, v->tail, d+1); fprintf(out, ")");
 }
 else fprintf(out, "efieldList()");
}




