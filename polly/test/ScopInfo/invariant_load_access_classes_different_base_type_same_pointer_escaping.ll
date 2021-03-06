; RUN: opt %loadPolly -polly-scops -analyze < %s | FileCheck %s
; RUN: opt %loadPolly -polly-codegen -S < %s | FileCheck %s --check-prefix=CODEGEN
;
;    int U;
;    int f(int *A) {
;      int i = 0, x, y;
;      do {
;        x = (*(int *)&U);
;        y = (int)(*(float *)&U);
;        A[i] = x + y;
;      } while (i++ < 100);
;      return x + y;
;    }
;
; CHECK:    Invariant Accesses: {
; CHECK-NOT:        ReadAccess
; CHECK:            ReadAccess := [Reduction Type: NONE] [Scalar: 0]
; CHECK:                { Stmt_do_body[i0] -> MemRef_U[0] };
; CHECK:            Execution Context: {  :  }
; CHECK-NOT:        ReadAccess
; CHECK:    }
;
; CHECK:    Statements {
; CHECK:      Stmt_do_body
; CHECK:            Domain :=
; CHECK:                { Stmt_do_body[i0] : i0 <= 100 and i0 >= 0 };
; CHECK:            Schedule :=
; CHECK:                { Stmt_do_body[i0] -> [i0] };
; CHECK:            MustWriteAccess :=  [Reduction Type: NONE] [Scalar: 0]
; CHECK:                { Stmt_do_body[i0] -> MemRef_A[i0] };
; CHECK:    }
;
; CODEGEN: entry:
; CODEGEN:   %U.f.preload.s2a = alloca float
; CODEGEN:   br label %polly.split_new_and_old
;
; CODEGEN: polly.preload.begin:
; CODEGEN:   %U.load1 = load float, float* bitcast (i32* @U to float*)
; CODEGEN:   %0 = bitcast float %U.load1 to i32
; CODEGEN:   store float %U.load1, float* %U.f.preload.s2a
;
; CODEGEN:     polly.merge_new_and_old:
; CODEGEN-DAG:   %U.f.merge = phi float [ %U.f.final_reload, %polly.exiting ], [ %U.f, %do.cond ]
; CODEGEN-DAG:   %U.i.merge = phi i32 [ %5, %polly.exiting ], [ %U.i, %do.cond ]
;
; CODEGEN: polly.loop_exit:
; CODEGEN-DAG:   %U.f.final_reload = load float, float* %U.f.preload.s2a
; CODEGEN-DAG:   %U.i.final_reload = load float, float* %U.f.preload.s2a
; CODEGEN-DAG:   %5 = bitcast float %U.i.final_reload to i32
;
; CODEGEN: polly.stmt.do.body:
; CODEGEN:   %p_conv = fptosi float %U.load1 to i32
; CODEGEN:   %p_add = add nsw i32 %0, %p_conv
;
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"

@U = common global i32 0, align 4

define i32 @f(i32* %A) {
entry:
  br label %do.body

do.body:                                          ; preds = %do.cond, %entry
  %indvars.iv = phi i64 [ %indvars.iv.next, %do.cond ], [ 0, %entry ]
  %U.i = load i32, i32* @U, align 4
  %U.cast = bitcast i32 *@U to float*
  %U.f = load float, float* %U.cast, align 4
  %conv = fptosi float %U.f to i32
  %add = add nsw i32 %U.i, %conv
  %arrayidx = getelementptr inbounds i32, i32* %A, i64 %indvars.iv
  store i32 %add, i32* %arrayidx, align 4
  br label %do.cond

do.cond:                                          ; preds = %do.body
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1
  %exitcond = icmp ne i64 %indvars.iv.next, 101
  br i1 %exitcond, label %do.body, label %do.end

do.end:                                           ; preds = %do.cond
  %conv2 = fptosi float %U.f to i32
  %add2 = add nsw i32 %U.i, %conv2
  ret i32 %add2
}
