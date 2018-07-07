; ModuleID = 'test_idl.c'
source_filename = "test_idl.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.person_t = type { i32, [10 x i8] }

@.str = private unnamed_addr constant [5 x i8] c"Jack\00", align 1
@.str.1 = private unnamed_addr constant [7 x i8] c"Age:%d\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define i32 @f1(%struct.person_t* %p, i32 %num) #0 {
entry:
  %p.addr = alloca %struct.person_t*, align 8
  %num.addr = alloca i32, align 4
  store %struct.person_t* %p, %struct.person_t** %p.addr, align 8
  store i32 %num, i32* %num.addr, align 4
  %0 = load %struct.person_t*, %struct.person_t** %p.addr, align 8
  %age = getelementptr inbounds %struct.person_t, %struct.person_t* %0, i32 0, i32 0
  %1 = load i32, i32* %age, align 4
  %2 = load i32, i32* %num.addr, align 4
  %add = add nsw i32 %1, %2
  ret i32 %add
}

; Function Attrs: noinline nounwind optnone uwtable
define i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %a = alloca i32, align 4
  %p = alloca %struct.person_t, align 4
  %b = alloca i32, align 4
  %c = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  store i32 5, i32* %a, align 4
  %age = getelementptr inbounds %struct.person_t, %struct.person_t* %p, i32 0, i32 0
  store i32 10, i32* %age, align 4
  %name = getelementptr inbounds %struct.person_t, %struct.person_t* %p, i32 0, i32 1
  %arraydecay = getelementptr inbounds [10 x i8], [10 x i8]* %name, i32 0, i32 0
  %call = call i8* @strncpy(i8* %arraydecay, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str, i32 0, i32 0), i64 4) #3
  %0 = load i32, i32* %a, align 4
  %call1 = call i32 @f1(%struct.person_t* %p, i32 %0)
  store i32 %call1, i32* %b, align 4
  %1 = load i32, i32* %a, align 4
  %2 = load i32, i32* %b, align 4
  %add = add nsw i32 %1, %2
  store i32 %add, i32* %c, align 4
  %age2 = getelementptr inbounds %struct.person_t, %struct.person_t* %p, i32 0, i32 0
  %3 = load i32, i32* %age2, align 4
  %call3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str.1, i32 0, i32 0), i32 %3)
  ret i32 0
}

; Function Attrs: nounwind
declare i8* @strncpy(i8*, i8*, i64) #1

declare i32 @printf(i8*, ...) #2

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 7.0.0 (trunk 323988) (llvm/trunk 323938)"}
