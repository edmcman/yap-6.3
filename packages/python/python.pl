%%% -*- Mode: Prolog; -*-
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%    Author:        Vitor Santos Costa
%    E-mail:        vsc@dcc.fc.up.pt
%    Copyright (C): Universidade do Porto
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  This file is part of the YAP Python Interface
%  distributed according to Perl Artistic License
%  check LICENSE file for distribution license
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/** <module> python

A C-based  Prolog interface to python.

@author		Vitor Santos Costa
@version	0:0:5, 2012/10/8
@license	Perl Artistic License

*/

%%%

:- module(python, [
     init_python/0,
     end_python/0,
     python_command/1,
     python_assign/3,
     python_import/1,
     python/2,
     op(100,fy,$),
     op(950,fy,:=),
     op(950,yfx,:=),
     (:=)/2,
     (:=)/1
     ]).


/************************************************************************************************************

Python interface

Data types are

     Python		Prolog
     string             atoms
     numbers		numbers
     lists		lists
     tuples             t(...)
     generic objs	__pointer__(Address)

     $var refers to the attribute __main__.var

*************************************************************************************************************/

:- use_module(library(shlib)).
:- use_module(library(lists)).
:- use_module(library(apply_macros)).
:- use_module(library(charsio)).

:- dynamic python_mref_cache/2.

:= F :- python(F,_).

V := F :- var(V), !, python(F,V).
A := F :- python(F, F1), python_assign(A, F1).

python_import(Module) :-
	python_do_import(Module, _).

python_do_import(Module, MRef) :-
	python_mref_cache(Module, MRef), !.
python_do_import(Module, MRef) :-
        python_import(Module, MRef),
	assert( python_mref_cache(Module, MRef) ).

fetch_module(M:E, M1, E1, MRef) :-
	atom(M),
	python_import(M, MRef0),
	module_extend(M, E, M1, E1, MRef0, MRef).

%
% extend the module as much as we can.
%
module_extend(M0, M:E, MF, EF, MRef0, MRef) :-
	atom(M),
	atom_concat([M0,'.',M], MM),
	python_import(MM, MRef1), !,
	module_extend(MM, E, MF, EF, MRef1, MRef).
module_extend(M, E, M, E, MRef, MRef).

% given an object, detect its len method
python_eval_term(Expression, O) :-
	fetch_module(Expression, Module, Exp, MRef), !,
	(
	 % avoid looking at : as field of module.
	    Exp = Obj:Field
	->
	    python_access(MRef, Exp, O)
	;
	    functor(Exp, F, _),
	    python_f(MRef, F, FRef),
	    python_check_args(FRef, Exp, NExp)
	->
	    python_apply(FRef, NExp, O)
	;
	    python_access(MRef, Exp, O)
	).
python_eval_term(Obj:Field, O) :- !,
	python_access(Obj, Field, O).
python_eval_term(Obj, O) :-
	python_is(Obj, O).


python_check_args(FRef, Exp, NExp) :-
	functor(Exp, _, Arity),
	arg(Arity, Exp, _=_), !,
	fetch_args(FRef, Dict),
	Exp =.. [F|LArgs],
	match_args(LArgs, Dict, NLArgs, _),
	NExp =.. [F|NLArgs].
python_check_args(FRef, Exp, Exp).

fetch_args(FRef, Args) :-
	python_import('inspect', M),
	python_f(M, getargspec, F),
	python_apply(F, getargspec(FRef), Args),
	ExtraArgs=t(Args, _, _, _).


match_args([], _, [], ok).
match_args([A=V|LArgs], Dict, [I=V|NLArgs], OK) :-
	match_args(LArgs, Dict, NLArgs,  ok), !,
	( nth0(I, Dict, A) -> true ; throw(type_error(argument(A=V))) ).
match_args([A|LArgs], Dict, [A|NLArgs], not_ok) :-
	match_args(LArgs, Dict, NLArgs, _).

python(Obj, Out) :-
	 python_eval_term(Obj, Out).

python_command(Cmd) :-
       python_run_command(Cmd).

start_python :-
	use_foreign_library(foreign(python)),
	init_python,
	python_command('import sys'),
	python_command('import inspect').

add_cwd_to_python :-
	unix(getcwd(Dir)),
	atom_concat(['sys.path.append(\"',Dir,'\")'], Command),
	python_command(Command),
	python_command("sys.argv = [\"yap\"]").
	% done	

python_assign(Name, Exp, '$'(Name)) :-
	python_assign(Name, Exp).

:- initialization(start_python, now).

:- initialization(add_cwd_to_python).

