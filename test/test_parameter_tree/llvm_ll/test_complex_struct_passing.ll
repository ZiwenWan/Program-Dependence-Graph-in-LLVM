; ModuleID = 'test_complex_struct_passing.c'
source_filename = "test_complex_struct_passing.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.clothes = type { [10 x i8], i32 }
%struct.person_t = type { i32, [10 x i8], %struct.clothes* }

@.str = private unnamed_addr constant [24 x i8] c"%s is wearing %s today.\00", align 1
@main.c = private unnamed_addr constant %struct.clothes { [10 x i8] c"red\00\00\00\00\00\00\00", i32 5 }, align 4
@.str.1 = private unnamed_addr constant [10 x i8] c"Jack\00\00\00\00\00\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define void @f(%struct.person_t* %p1) #0 {
entry:
  %p1.addr = alloca %struct.person_t*, align 8
  %name = alloca i8*, align 8
  %color = alloca i8*, align 8
  store %struct.person_t* %p1, %struct.person_t** %p1.addr, align 8
  %0 = load %struct.person_t*, %struct.person_t** %p1.addr, align 8
  %name1 = getelementptr inbounds %struct.person_t, %struct.person_t* %0, i32 0, i32 1
  %arraydecay = getelementptr inbounds [10 x i8], [10 x i8]* %name1, i32 0, i32 0
  store i8* %arraydecay, i8** %name, align 8
  %1 = load %struct.person_t*, %struct.person_t** %p1.addr, align 8
  %s = getelementptr inbounds %struct.person_t, %struct.person_t* %1, i32 0, i32 2
  %2 = load %struct.clothes*, %struct.clothes** %s, align 8
  %color2 = getelementptr inbounds %struct.clothes, %struct.clothes* %2, i32 0, i32 0
  %arraydecay3 = getelementptr inbounds [10 x i8], [10 x i8]* %color2, i32 0, i32 0
  store i8* %arraydecay3, i8** %color, align 8
  %3 = load i8*, i8** %name, align 8
  %4 = load i8*, i8** %color, align 8
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str, i32 0, i32 0), i8* %3, i8* %4)
  ret void
}

declare i32 @printf(i8*, ...) #1

; Function Attrs: noinline nounwind optnone uwtable
define i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %c = alloca %struct.clothes, align 4
  %p = alloca %struct.person_t, align 8
  %pt = alloca %struct.person_t*, align 8
  store i32 0, i32* %retval, align 4
  %0 = bitcast %struct.clothes* %c to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 4 %0, i8* align 4 getelementptr inbounds (%struct.clothes, %struct.clothes* @main.c, i32 0, i32 0, i32 0), i64 16, i1 false)
  %age = getelementptr inbounds %struct.person_t, %struct.person_t* %p, i32 0, i32 0
  store i32 10, i32* %age, align 8
  %name = getelementptr inbounds %struct.person_t, %struct.person_t* %p, i32 0, i32 1
  %1 = bitcast [10 x i8]* %name to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 4 %1, i8* align 1 getelementptr inbounds ([10 x i8], [10 x i8]* @.str.1, i32 0, i32 0), i64 10, i1 false)
  %s = getelementptr inbounds %struct.person_t, %struct.person_t* %p, i32 0, i32 2
  store %struct.clothes* %c, %struct.clothes** %s, align 8
  store %struct.person_t* %p, %struct.person_t** %pt, align 8
  %2 = load %struct.person_t*, %struct.person_t** %pt, align 8
  call void @f(%struct.person_t* %2)
  ret i32 0
}

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i1) #2

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { argmemonly nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 7.0.0 (trunk 323988) (llvm/trunk 323938)"}
