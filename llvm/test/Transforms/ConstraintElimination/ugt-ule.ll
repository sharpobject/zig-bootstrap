; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt -constraint-elimination -S %s | FileCheck %s

declare void @use(i1)

define void @test(i8* %m, i8* %ptr) {
; CHECK-LABEL: @test(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[CMP_1:%.*]] = icmp ult i8* [[M:%.*]], [[PTR:%.*]]
; CHECK-NEXT:    br i1 [[CMP_1]], label [[BB_1:%.*]], label [[BB_2:%.*]]
; CHECK:       bb.1:
; CHECK-NEXT:    [[CMP_2:%.*]] = icmp uge i8* [[M]], [[PTR]]
; CHECK-NEXT:    call void @use(i1 false)
; CHECK-NEXT:    ret void
; CHECK:       bb.2:
; CHECK-NEXT:    br label [[BB_2_NEXT:%.*]]
; CHECK:       bb.2.next:
; CHECK-NEXT:    [[CMP_3:%.*]] = icmp uge i8* [[M]], [[PTR]]
; CHECK-NEXT:    call void @use(i1 true)
; CHECK-NEXT:    ret void
;
entry:
  %cmp.1 = icmp ult i8* %m, %ptr
  br i1 %cmp.1, label %bb.1, label %bb.2

bb.1:
  %cmp.2 = icmp uge i8* %m, %ptr
  call void @use(i1 %cmp.2)
  ret void

bb.2:
  br label %bb.2.next

bb.2.next:
  %cmp.3 = icmp uge i8* %m, %ptr
  call void @use(i1 %cmp.3)
  ret void
}