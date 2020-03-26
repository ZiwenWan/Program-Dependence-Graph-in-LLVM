; ModuleID = 'test_field_sensitive_pdg.c'
source_filename = "test_field_sensitive_pdg.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.Person = type { i32, i8* }

@.str = private unnamed_addr constant [4 x i8] c"Doo\00", align 1
@main.p = private unnamed_addr constant %struct.Person { i32 20, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0) }, align 8
@Person = common global %struct.Person zeroinitializer, align 8

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @f(%struct.Person*) #0 {
  %2 = alloca %struct.Person*, align 8
  store %struct.Person* %0, %struct.Person** %2, align 8
  %3 = load %struct.Person*, %struct.Person** %2, align 8
  %4 = getelementptr inbounds %struct.Person, %struct.Person* %3, i32 0, i32 0
  %5 = load i32, i32* %4, align 8
  ret i32 %5
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca %struct.Person, align 8
  store i32 0, i32* %1, align 4
  %3 = bitcast %struct.Person* %2 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %3, i8* bitcast (%struct.Person* @main.p to i8*), i64 16, i32 8, i1 false)
  %4 = call i32 @f(%struct.Person* %2)
  ret i32 0
}

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i32, i1) #1

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
