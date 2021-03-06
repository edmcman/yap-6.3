dnl
dnl JIT CONFIGURATION
dnl

AC_ARG_VAR( LLVM_CONFIG, [ full path to llvm-config program ])


AC_SUBST(JITFLAGS)
AC_SUBST(JITLD)
AC_SUBST(JITLIBS)
AC_SUBST(JITDEBUGPREDS)
AC_SUBST(JITSTATISTICPREDS)
AC_SUBST(JITCOMPILER)
AC_SUBST(JITCONFIGPREDS)
AC_SUBST(JITANALYSISPREDS)
AC_SUBST(JITTRANSFORMPREDS)
AC_SUBST(JITCODEGENPREDS)
AC_SUBST(PAPILIB)

AC_ARG_ENABLE(jit,
	      [ --enable-jit                   support just-in-time (JIT) compilation],
	      yap_jit="$enableval", yap_jit=no)

AC_ARG_ENABLE(debug-predicates,
	      [ --enable-debug-predicates      support debug predicates ],
	      dbg_preds="$enableval", dbg_preds=no)

AC_ARG_ENABLE(statistic-predicates,
	      [ --enable-statistic-predicates  support statistic predicates ],
	      stat_preds="$enableval", stat_preds=no)

if test "$yap_jit" = "yes"
then
    #assumes we have r on path
    AC_CHECK_PROGS(LLVM_CONFIG, llvm-config , "no"  )
    AC_CHECK_PROGS(CLANG, clang , [no] )
elif test "$yap_jit" = "no"
then
    LLVM_CONFIG=no
    CLANG=no
else
    AC_PATH_PROG(LLVM_CONFIG, llvm-config  ,"no", [ "$yap_jit"/bin ] )
    AC_PATH_PROG(CLANG, clang, "no", [ "$yap_jit"/bin ] )
fi

if test "$LLVM_CONFIG" = "no" ;then
    AC_MSG_ERROR([--enable-jit was given, but test for LLVM 3.5 failed])
else
    LLVM_VERSION="`$LLVM_CONFIG --version`"
    if test "$LLVM_VERSION" != "3.5.0";then
        AC_MSG_ERROR([Test for LLVM 3.5 failed])
    fi

    if test "$CLANG" = "no" ;then
	AC_MSG_ERROR([--enable-jit was given, but test for clang faild])
    fi

    YAP_EXTRAS="$YAP_EXTRAS -DYAP_JIT=1"
    JITCOMPILER="JIT_Compiler.o"
    JITCONFIGPREDS="jit_configpreds.o"
    JITANALYSISPREDS="jit_analysispreds.o"
    JITTRANSFORMPREDS="jit_transformpreds.o"
    JITCODEGENPREDS="jit_codegenpreds.o"
    JITFLAGS="`$LLVM_CONFIG --cxxflags`"
    JITLD="`$LLVM_CONFIG --ldflags`"
    JITLIBS="`$LLVM_CONFIG --libs all` -pthread -lffi -lz"

fi

if test "$dbg_preds" = "yes"
then

    if test "$yap_jit" = "no"
    then

	AC_MSG_ERROR([--enable-debug-predicates was given, but --enable-jit was not given])

    fi

    YAP_EXTRAS="$YAP_EXTRAS -DYAP_DBG_PREDS=1"
    JITDEBUGPREDS="jit_debugpreds.o"
fi

if test "$stat_preds" = "yes"
then

    if test "$yap_jit" = "no"
    then

	AC_MSG_ERROR([--enable-statistic-predicates was given, but --enable-jit was not given])

    fi

    AC_CHECK_HEADER([papi.h],
                    [],
		    [if test "$stat_preds" != "no"; then
			 AC_MSG_ERROR(
			     [--enable-statistic-predicates was given, but papi.h not found])
                     fi
                    ])

    AC_CHECK_LIB([papi], [PAPI_start],
		 [if test "$stat_preds" != "no"; then
		      PAPILIB="-lpapi"
		  fi
		 ],
		 [if test "$stat_preds" != "no"; then
                      AC_MSG_ERROR(
			  [--enable-statistic-predicates was given, but test for papi failed])
		  fi
		 ])

    YAP_EXTRAS="$YAP_EXTRAS -DYAP_STAT_PREDS=1"
    JITSTATISTICPREDS="jit_statisticpreds.o"
    PAPILIB="-lpapi"

fi
