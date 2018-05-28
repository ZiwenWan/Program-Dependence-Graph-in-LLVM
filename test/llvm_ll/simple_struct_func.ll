; ModuleID = 'simple_struct_func.c'
source_filename = "simple_struct_func.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.S1 = type { i32, float }

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define void @print(%struct.S1* %p) #0 {
entry:
  %p.addr = alloca %struct.S1*, align 8
  store %struct.S1* %p, %struct.S1** %p.addr, align 8
  %0 = load %struct.S1*, %struct.S1** %p.addr, align 8
  %a = getelementptr inbounds %struct.S1, %struct.S1* %0, i32 0, i32 0
  %1 = load i32, i32* %a, align 4
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %1)
  ret void
}

declare i32 @printf(i8*, ...) #1

; Function Attrs: noinline nounwind optnone uwtable
define i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %s1 = alloca %struct.S1*, align 8
  store i32 0, i32* %retval, align 4
  %0 = load %struct.S1*, %struct.S1** %s1, align 8
  %a = getelementptr inbounds %struct.S1, %struct.S1* %0, i32 0, i32 0
  store i32 5, i32* %a, align 4
  %1 = load %struct.S1*, %struct.S1** %s1, align 8
  %b = getelementptr inbounds %struct.S1, %struct.S1* %1, i32 0, i32 1
  store float 0x3FF547AE20000000, float* %b, align 4
  %2 = load %struct.S1*, %struct.S1** %s1, align 8
  call void @print(%struct.S1* %2)
  ret i32 0
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 7.0.0 (trunk 323988) (llvm/trunk 323938)"}
