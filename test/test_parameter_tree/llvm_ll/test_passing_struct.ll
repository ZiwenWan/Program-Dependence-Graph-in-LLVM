; ModuleID = 'test_passing_struct.c'
source_filename = "test_passing_struct.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.P = type { i32, [10 x i8] }

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define i32 @f(%struct.P* %a, %struct.P* %b) #0 {
entry:
  %a.addr = alloca %struct.P*, align 8
  %b.addr = alloca %struct.P*, align 8
  store %struct.P* %a, %struct.P** %a.addr, align 8
  store %struct.P* %b, %struct.P** %b.addr, align 8
  %0 = load %struct.P*, %struct.P** %a.addr, align 8
  %age = getelementptr inbounds %struct.P, %struct.P* %0, i32 0, i32 0
  %1 = load i32, i32* %age, align 4
  %2 = load %struct.P*, %struct.P** %b.addr, align 8
  %age1 = getelementptr inbounds %struct.P, %struct.P* %2, i32 0, i32 0
  %3 = load i32, i32* %age1, align 4
  %add = add nsw i32 %1, %3
  ret i32 %add
}

; Function Attrs: noinline nounwind optnone uwtable
define i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %a = alloca %struct.P, align 4
  %b = alloca %struct.P, align 4
  store i32 0, i32* %retval, align 4
  %age = getelementptr inbounds %struct.P, %struct.P* %a, i32 0, i32 0
  store i32 10, i32* %age, align 4
  %age1 = getelementptr inbounds %struct.P, %struct.P* %b, i32 0, i32 0
  store i32 10, i32* %age1, align 4
  %call = call i32 @f(%struct.P* %a, %struct.P* %b)
  %call2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %call)
  ret i32 0
}

declare i32 @printf(i8*, ...) #1

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 7.0.0 (trunk 323988) (llvm/trunk 323938)"}
