; ModuleID = 'test_field_sensitive_pdg.c'
source_filename = "test_field_sensitive_pdg.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.15.0"

%struct.Person = type { i32, i8* }

@.str = private unnamed_addr constant [4 x i8] c"Doo\00", align 1
@main.p = private unnamed_addr constant %struct.Person { i32 20, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0) }, align 8
@Person = common global %struct.Person zeroinitializer, align 8, !dbg !0

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @f(%struct.Person*) #0 !dbg !18 {
  %2 = alloca %struct.Person*, align 8
  store %struct.Person* %0, %struct.Person** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.Person** %2, metadata !22, metadata !23), !dbg !24
  %3 = load %struct.Person*, %struct.Person** %2, align 8, !dbg !25
  %4 = getelementptr inbounds %struct.Person, %struct.Person* %3, i32 0, i32 0, !dbg !26
  %5 = load i32, i32* %4, align 8, !dbg !26
  ret i32 %5, !dbg !27
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !28 {
  %1 = alloca i32, align 4
  %2 = alloca %struct.Person, align 8
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata %struct.Person* %2, metadata !31, metadata !23), !dbg !32
  %3 = bitcast %struct.Person* %2 to i8*, !dbg !32
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %3, i8* bitcast (%struct.Person* @main.p to i8*), i64 16, i32 8, i1 false), !dbg !32
  %4 = call i32 @f(%struct.Person* %2), !dbg !33
  ret i32 0, !dbg !34
}

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i32, i1) #2

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { argmemonly nounwind }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!13, !14, !15, !16}
!llvm.ident = !{!17}

!0 = !DIGlobalVariableExpression(var: !1)
!1 = distinct !DIGlobalVariable(name: "Person", scope: !2, file: !3, line: 5, type: !6, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 5.0.2 (tags/RELEASE_502/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, globals: !5)
!3 = !DIFile(filename: "test_field_sensitive_pdg.c", directory: "/Users/yongzhehuang/Documents/llvm_versions/program-dependence-graph-in-llvm/test/pdg_test")
!4 = !{}
!5 = !{!0}
!6 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "Person", file: !3, line: 2, size: 128, elements: !7)
!7 = !{!8, !10}
!8 = !DIDerivedType(tag: DW_TAG_member, name: "age", scope: !6, file: !3, line: 3, baseType: !9, size: 32)
!9 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!10 = !DIDerivedType(tag: DW_TAG_member, name: "name", scope: !6, file: !3, line: 4, baseType: !11, size: 64, offset: 64)
!11 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !12, size: 64)
!12 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!13 = !{i32 2, !"Dwarf Version", i32 4}
!14 = !{i32 2, !"Debug Info Version", i32 3}
!15 = !{i32 1, !"wchar_size", i32 4}
!16 = !{i32 7, !"PIC Level", i32 2}
!17 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
!18 = distinct !DISubprogram(name: "f", scope: !3, file: !3, line: 7, type: !19, isLocal: false, isDefinition: true, scopeLine: 7, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!19 = !DISubroutineType(types: !20)
!20 = !{!9, !21}
!21 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !6, size: 64)
!22 = !DILocalVariable(name: "p", arg: 1, scope: !18, file: !3, line: 7, type: !21)
!23 = !DIExpression()
!24 = !DILocation(line: 7, column: 22, scope: !18)
!25 = !DILocation(line: 8, column: 12, scope: !18)
!26 = !DILocation(line: 8, column: 15, scope: !18)
!27 = !DILocation(line: 8, column: 5, scope: !18)
!28 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 11, type: !29, isLocal: false, isDefinition: true, scopeLine: 11, isOptimized: false, unit: !2, variables: !4)
!29 = !DISubroutineType(types: !30)
!30 = !{!9}
!31 = !DILocalVariable(name: "p", scope: !28, file: !3, line: 12, type: !6)
!32 = !DILocation(line: 12, column: 19, scope: !28)
!33 = !DILocation(line: 13, column: 5, scope: !28)
!34 = !DILocation(line: 14, column: 5, scope: !28)
