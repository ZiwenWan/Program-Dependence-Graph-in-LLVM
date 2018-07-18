; ModuleID = 'test_two_source_data_dependency.c'
source_filename = "test_two_source_data_dependency.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.struct_t = type { i32, float }

; Function Attrs: noinline nounwind optnone uwtable
define i32 @getFlag() #0 {
entry:
  ret i32 1
}

; Function Attrs: noinline nounwind optnone uwtable
define i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %s = alloca %struct.struct_t, align 4
  %a = alloca i32, align 4
  %b = alloca float, align 4
  store i32 0, i32* %retval, align 4
  store i32 5, i32* %a, align 4
  store float 6.000000e+00, float* %b, align 4
  %call = call i32 @getFlag()
  %tobool = icmp ne i32 %call, 0
  br i1 %tobool, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %0 = load i32, i32* %a, align 4
  %a1 = getelementptr inbounds %struct.struct_t, %struct.struct_t* %s, i32 0, i32 0
  store i32 %0, i32* %a1, align 4
  br label %if.end

if.else:                                          ; preds = %entry
  %1 = load float, float* %b, align 4
  %b2 = getelementptr inbounds %struct.struct_t, %struct.struct_t* %s, i32 0, i32 1
  store float %1, float* %b2, align 4
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  ret i32 0
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 7.0.0 (trunk 323988) (llvm/trunk 323938)"}
