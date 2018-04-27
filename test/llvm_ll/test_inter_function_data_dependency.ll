; ModuleID = 'test_inter_function_data_dependency.c'
source_filename = "test_inter_function_data_dependency.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [20 x i8] c"int: %d, float: %f\0A\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define void @f2(i32 %a, float %b) #0 {
entry:
  %a.addr = alloca i32, align 4
  %b.addr = alloca float, align 4
  store i32 %a, i32* %a.addr, align 4
  store float %b, float* %b.addr, align 4
  %0 = load i32, i32* %a.addr, align 4
  %1 = load float, float* %b.addr, align 4
  %conv = fpext float %1 to double
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([20 x i8], [20 x i8]* @.str, i32 0, i32 0), i32 %0, double %conv)
  ret void
}

declare i32 @printf(i8*, ...) #1

; Function Attrs: noinline nounwind optnone uwtable
define void @f1(i32 %a, float %b) #0 {
entry:
  %a.addr = alloca i32, align 4
  %b.addr = alloca float, align 4
  store i32 %a, i32* %a.addr, align 4
  store float %b, float* %b.addr, align 4
  %0 = load i32, i32* %a.addr, align 4
  %1 = load float, float* %b.addr, align 4
  call void @f2(i32 %0, float %1)
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca float, align 4
  store i32 0, i32* %retval, align 4
  store i32 5, i32* %a, align 4
  store float 5.500000e+00, float* %b, align 4
  %0 = load i32, i32* %a, align 4
  %1 = load float, float* %b, align 4
  call void @f1(i32 %0, float %1)
  ret i32 0
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 7.0.0 (trunk 323988) (llvm/trunk 323938)"}
