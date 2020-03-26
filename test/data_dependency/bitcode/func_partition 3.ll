; ModuleID = 'func_partition.bc'
source_filename = "func_partition.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct._IO_FILE = type { i32, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, %struct._IO_marker*, %struct._IO_FILE*, i32, i32, i64, i16, i8, [1 x i8], i8*, i64, i8*, i8*, i8*, i8*, i64, i32, [20 x i8] }
%struct._IO_marker = type { %struct._IO_marker*, %struct._IO_FILE*, i32 }
%struct.passwd = type { i8*, i8*, i8* }

@.str = private unnamed_addr constant [7 x i8] c"passwd\00", align 1
@.str.1 = private unnamed_addr constant [2 x i8] c"r\00", align 1
@stderr = external global %struct._IO_FILE*, align 8
@.str.2 = private unnamed_addr constant [24 x i8] c"Unable to open passwd!\0A\00", align 1
@.str.3 = private unnamed_addr constant [16 x i8] c"Out of memory!\0A\00", align 1
@.str.4 = private unnamed_addr constant [23 x i8] c"Error reading passwd!\0A\00", align 1
@.str.5 = private unnamed_addr constant [3 x i8] c" \09\00", align 1
@.str.6 = private unnamed_addr constant [10 x i8] c"sensitive\00", section "llvm.metadata"
@.str.7 = private unnamed_addr constant [17 x i8] c"func_partition.c\00", section "llvm.metadata"
@.str.8 = private unnamed_addr constant [30 x i8] c"Usage: %s uid home_dir shell\0A\00", align 1
@.str.9 = private unnamed_addr constant [8 x i8] c"Error!\0A\00", align 1

; Function Attrs: noinline nounwind uwtable
define %struct.passwd* @pw_locate(i8* %id) #0 !dbg !16 {
entry:
  %retval = alloca %struct.passwd*, align 8
  %id.addr = alloca i8*, align 8
  %fp = alloca %struct._IO_FILE*, align 8
  %buf = alloca [256 x i8], align 16
  %pw = alloca %struct.passwd*, align 8
  %p = alloca i8*, align 8
  store i8* %id, i8** %id.addr, align 8
  call void @llvm.dbg.declare(metadata i8** %id.addr, metadata !21, metadata !22), !dbg !23
  call void @llvm.dbg.declare(metadata %struct._IO_FILE** %fp, metadata !24, metadata !22), !dbg !85
  %call = call %struct._IO_FILE* @fopen(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str, i32 0, i32 0), i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.1, i32 0, i32 0)), !dbg !86
  store %struct._IO_FILE* %call, %struct._IO_FILE** %fp, align 8, !dbg !85
  %0 = load %struct._IO_FILE*, %struct._IO_FILE** %fp, align 8, !dbg !87
  %cmp = icmp eq %struct._IO_FILE* %0, null, !dbg !89
  br i1 %cmp, label %if.then, label %if.end, !dbg !90

if.then:                                          ; preds = %entry
  %1 = load %struct._IO_FILE*, %struct._IO_FILE** @stderr, align 8, !dbg !91
  %call1 = call i32 (%struct._IO_FILE*, i8*, ...) @fprintf(%struct._IO_FILE* %1, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str.2, i32 0, i32 0)), !dbg !93
  store %struct.passwd* null, %struct.passwd** %retval, align 8, !dbg !94
  br label %return, !dbg !94

if.end:                                           ; preds = %entry
  call void @llvm.dbg.declare(metadata [256 x i8]* %buf, metadata !95, metadata !22), !dbg !99
  call void @llvm.dbg.declare(metadata %struct.passwd** %pw, metadata !100, metadata !22), !dbg !101
  %call2 = call noalias i8* @malloc(i64 24) #5, !dbg !102
  %2 = bitcast i8* %call2 to %struct.passwd*, !dbg !103
  store %struct.passwd* %2, %struct.passwd** %pw, align 8, !dbg !101
  %3 = load %struct.passwd*, %struct.passwd** %pw, align 8, !dbg !104
  %cmp3 = icmp eq %struct.passwd* %3, null, !dbg !106
  br i1 %cmp3, label %if.then4, label %if.end6, !dbg !107

if.then4:                                         ; preds = %if.end
  %4 = load %struct._IO_FILE*, %struct._IO_FILE** @stderr, align 8, !dbg !108
  %call5 = call i32 (%struct._IO_FILE*, i8*, ...) @fprintf(%struct._IO_FILE* %4, i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.3, i32 0, i32 0)), !dbg !110
  store %struct.passwd* null, %struct.passwd** %retval, align 8, !dbg !111
  br label %return, !dbg !111

if.end6:                                          ; preds = %if.end
  br label %while.cond, !dbg !112

while.cond:                                       ; preds = %if.end35, %if.else, %if.end6
  %5 = load %struct._IO_FILE*, %struct._IO_FILE** %fp, align 8, !dbg !113
  %call7 = call i32 @feof(%struct._IO_FILE* %5) #5, !dbg !115
  %tobool = icmp ne i32 %call7, 0, !dbg !116
  %lnot = xor i1 %tobool, true, !dbg !116
  br i1 %lnot, label %while.body, label %while.end, !dbg !117

while.body:                                       ; preds = %while.cond
  %arraydecay = getelementptr inbounds [256 x i8], [256 x i8]* %buf, i32 0, i32 0, !dbg !118
  %6 = load %struct._IO_FILE*, %struct._IO_FILE** %fp, align 8, !dbg !121
  %call8 = call i8* @fgets(i8* %arraydecay, i32 256, %struct._IO_FILE* %6), !dbg !122
  %cmp9 = icmp eq i8* %call8, null, !dbg !123
  br i1 %cmp9, label %if.then10, label %if.end12, !dbg !124

if.then10:                                        ; preds = %while.body
  %7 = load %struct._IO_FILE*, %struct._IO_FILE** @stderr, align 8, !dbg !125
  %call11 = call i32 (%struct._IO_FILE*, i8*, ...) @fprintf(%struct._IO_FILE* %7, i8* getelementptr inbounds ([23 x i8], [23 x i8]* @.str.4, i32 0, i32 0)), !dbg !127
  br label %while.end, !dbg !128

if.end12:                                         ; preds = %while.body
  call void @llvm.dbg.declare(metadata i8** %p, metadata !129, metadata !22), !dbg !130
  %arraydecay13 = getelementptr inbounds [256 x i8], [256 x i8]* %buf, i32 0, i32 0, !dbg !131
  %call14 = call i8* @strtok(i8* %arraydecay13, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.5, i32 0, i32 0)) #5, !dbg !132
  store i8* %call14, i8** %p, align 8, !dbg !130
  %8 = load i8*, i8** %p, align 8, !dbg !133
  %tobool15 = icmp ne i8* %8, null, !dbg !133
  br i1 %tobool15, label %if.then16, label %if.else, !dbg !135

if.then16:                                        ; preds = %if.end12
  %9 = load i8*, i8** %p, align 8, !dbg !136
  %call17 = call noalias i8* @strdup(i8* %9) #5, !dbg !137
  %10 = load %struct.passwd*, %struct.passwd** %pw, align 8, !dbg !138
  %pw_id = getelementptr inbounds %struct.passwd, %struct.passwd* %10, i32 0, i32 0, !dbg !139
  store i8* %call17, i8** %pw_id, align 8, !dbg !140
  br label %if.end18, !dbg !138

if.else:                                          ; preds = %if.end12
  br label %while.cond, !dbg !141, !llvm.loop !142

if.end18:                                         ; preds = %if.then16
  %11 = load %struct.passwd*, %struct.passwd** %pw, align 8, !dbg !144
  %pw_id19 = getelementptr inbounds %struct.passwd, %struct.passwd* %11, i32 0, i32 0, !dbg !146
  %12 = load i8*, i8** %pw_id19, align 8, !dbg !146
  %13 = load i8*, i8** %id.addr, align 8, !dbg !147
  %call20 = call i32 @strcmp(i8* %12, i8* %13) #8, !dbg !148
  %cmp21 = icmp eq i32 %call20, 0, !dbg !149
  br i1 %cmp21, label %if.then22, label %if.end35, !dbg !150

if.then22:                                        ; preds = %if.end18
  %call23 = call i8* @strtok(i8* null, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.5, i32 0, i32 0)) #5, !dbg !151
  store i8* %call23, i8** %p, align 8, !dbg !153
  %14 = load i8*, i8** %p, align 8, !dbg !154
  %tobool24 = icmp ne i8* %14, null, !dbg !154
  br i1 %tobool24, label %if.then25, label %if.else27, !dbg !156

if.then25:                                        ; preds = %if.then22
  %15 = load i8*, i8** %p, align 8, !dbg !157
  %call26 = call noalias i8* @strdup(i8* %15) #5, !dbg !158
  %16 = load %struct.passwd*, %struct.passwd** %pw, align 8, !dbg !159
  %pw_dir = getelementptr inbounds %struct.passwd, %struct.passwd* %16, i32 0, i32 1, !dbg !160
  store i8* %call26, i8** %pw_dir, align 8, !dbg !161
  br label %if.end28, !dbg !159

if.else27:                                        ; preds = %if.then22
  br label %while.end, !dbg !162

if.end28:                                         ; preds = %if.then25
  %call29 = call i8* @strtok(i8* null, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.5, i32 0, i32 0)) #5, !dbg !163
  store i8* %call29, i8** %p, align 8, !dbg !164
  %17 = load i8*, i8** %p, align 8, !dbg !165
  %tobool30 = icmp ne i8* %17, null, !dbg !165
  br i1 %tobool30, label %if.then31, label %if.else33, !dbg !167

if.then31:                                        ; preds = %if.end28
  %18 = load i8*, i8** %p, align 8, !dbg !168
  %call32 = call noalias i8* @strdup(i8* %18) #5, !dbg !169
  %19 = load %struct.passwd*, %struct.passwd** %pw, align 8, !dbg !170
  %pw_shell = getelementptr inbounds %struct.passwd, %struct.passwd* %19, i32 0, i32 2, !dbg !171
  store i8* %call32, i8** %pw_shell, align 8, !dbg !172
  br label %if.end34, !dbg !170

if.else33:                                        ; preds = %if.end28
  br label %while.end, !dbg !173

if.end34:                                         ; preds = %if.then31
  %20 = load %struct.passwd*, %struct.passwd** %pw, align 8, !dbg !174
  store %struct.passwd* %20, %struct.passwd** %retval, align 8, !dbg !175
  br label %return, !dbg !175

if.end35:                                         ; preds = %if.end18
  br label %while.cond, !dbg !176, !llvm.loop !142

while.end:                                        ; preds = %if.else33, %if.else27, %if.then10, %while.cond
  %21 = load %struct.passwd*, %struct.passwd** %pw, align 8, !dbg !178
  %22 = bitcast %struct.passwd* %21 to i8*, !dbg !178
  call void @free(i8* %22) #5, !dbg !179
  store %struct.passwd* null, %struct.passwd** %retval, align 8, !dbg !180
  br label %return, !dbg !180

return:                                           ; preds = %while.end, %if.end34, %if.then4, %if.then
  %23 = load %struct.passwd*, %struct.passwd** %retval, align 8, !dbg !181
  ret %struct.passwd* %23, !dbg !181
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare %struct._IO_FILE* @fopen(i8*, i8*) #2

declare i32 @fprintf(%struct._IO_FILE*, i8*, ...) #2

; Function Attrs: nounwind
declare noalias i8* @malloc(i64) #3

; Function Attrs: nounwind
declare i32 @feof(%struct._IO_FILE*) #3

declare i8* @fgets(i8*, i32, %struct._IO_FILE*) #2

; Function Attrs: nounwind
declare i8* @strtok(i8*, i8*) #3

; Function Attrs: nounwind
declare noalias i8* @strdup(i8*) #3

; Function Attrs: nounwind readonly
declare i32 @strcmp(i8*, i8*) #4

; Function Attrs: nounwind
declare void @free(i8*) #3

; Function Attrs: noinline nounwind uwtable
define void @pw_update(%struct.passwd* %pw) #0 !dbg !182 {
entry:
  %pw.addr = alloca %struct.passwd*, align 8
  store %struct.passwd* %pw, %struct.passwd** %pw.addr, align 8
  call void @llvm.dbg.declare(metadata %struct.passwd** %pw.addr, metadata !187, metadata !22), !dbg !188
  ret void, !dbg !189
}

; Function Attrs: noinline nounwind uwtable
define i32 @main(i32 %argc, i8** %argv) #0 !dbg !190 {
entry:
  %retval = alloca i32, align 4
  %argc.addr = alloca i32, align 4
  %argv.addr = alloca i8**, align 8
  %pw = alloca %struct.passwd*, align 8
  %newpw = alloca %struct.passwd, align 8
  %fields = alloca [3 x i8*], align 16
  %i = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  store i32 %argc, i32* %argc.addr, align 4
  call void @llvm.dbg.declare(metadata i32* %argc.addr, metadata !194, metadata !22), !dbg !195
  store i8** %argv, i8*** %argv.addr, align 8
  call void @llvm.dbg.declare(metadata i8*** %argv.addr, metadata !196, metadata !22), !dbg !197
  call void @llvm.dbg.declare(metadata %struct.passwd** %pw, metadata !198, metadata !22), !dbg !199
  %pw1 = bitcast %struct.passwd** %pw to i8*, !dbg !200
  call void @llvm.var.annotation(i8* %pw1, i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str.6, i32 0, i32 0), i8* getelementptr inbounds ([17 x i8], [17 x i8]* @.str.7, i32 0, i32 0), i32 55), !dbg !200
  call void @llvm.dbg.declare(metadata %struct.passwd* %newpw, metadata !201, metadata !22), !dbg !202
  call void @llvm.dbg.declare(metadata [3 x i8*]* %fields, metadata !203, metadata !22), !dbg !207
  %0 = load i32, i32* %argc.addr, align 4, !dbg !208
  %cmp = icmp ne i32 %0, 4, !dbg !210
  br i1 %cmp, label %if.then, label %if.end, !dbg !211

if.then:                                          ; preds = %entry
  %1 = load i8**, i8*** %argv.addr, align 8, !dbg !212
  %arrayidx = getelementptr inbounds i8*, i8** %1, i64 0, !dbg !212
  %2 = load i8*, i8** %arrayidx, align 8, !dbg !212
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([30 x i8], [30 x i8]* @.str.8, i32 0, i32 0), i8* %2), !dbg !214
  call void @exit(i32 0) #9, !dbg !215
  unreachable, !dbg !215

if.end:                                           ; preds = %entry
  call void @llvm.dbg.declare(metadata i32* %i, metadata !216, metadata !22), !dbg !217
  store i32 1, i32* %i, align 4, !dbg !218
  br label %for.cond, !dbg !220

for.cond:                                         ; preds = %for.inc, %if.end
  %3 = load i32, i32* %i, align 4, !dbg !221
  %4 = load i32, i32* %argc.addr, align 4, !dbg !224
  %cmp2 = icmp slt i32 %3, %4, !dbg !225
  br i1 %cmp2, label %for.body, label %for.end, !dbg !226

for.body:                                         ; preds = %for.cond
  %5 = load i8**, i8*** %argv.addr, align 8, !dbg !228
  %6 = load i32, i32* %i, align 4, !dbg !230
  %idxprom = sext i32 %6 to i64, !dbg !228
  %arrayidx3 = getelementptr inbounds i8*, i8** %5, i64 %idxprom, !dbg !228
  %7 = load i8*, i8** %arrayidx3, align 8, !dbg !228
  %call4 = call noalias i8* @strdup(i8* %7) #5, !dbg !231
  %8 = load i32, i32* %i, align 4, !dbg !232
  %sub = sub nsw i32 %8, 1, !dbg !233
  %idxprom5 = sext i32 %sub to i64, !dbg !234
  %arrayidx6 = getelementptr inbounds [3 x i8*], [3 x i8*]* %fields, i64 0, i64 %idxprom5, !dbg !234
  store i8* %call4, i8** %arrayidx6, align 8, !dbg !235
  br label %for.inc, !dbg !236

for.inc:                                          ; preds = %for.body
  %9 = load i32, i32* %i, align 4, !dbg !237
  %inc = add nsw i32 %9, 1, !dbg !237
  store i32 %inc, i32* %i, align 4, !dbg !237
  br label %for.cond, !dbg !239, !llvm.loop !240

for.end:                                          ; preds = %for.cond
  %arrayidx7 = getelementptr inbounds [3 x i8*], [3 x i8*]* %fields, i64 0, i64 0, !dbg !243
  %10 = load i8*, i8** %arrayidx7, align 16, !dbg !243
  %call8 = call %struct.passwd* @pw_locate(i8* %10), !dbg !244
  store %struct.passwd* %call8, %struct.passwd** %pw, align 8, !dbg !245
  %11 = load %struct.passwd*, %struct.passwd** %pw, align 8, !dbg !246
  %cmp9 = icmp eq %struct.passwd* %11, null, !dbg !248
  br i1 %cmp9, label %if.then10, label %if.end12, !dbg !249

if.then10:                                        ; preds = %for.end
  %12 = load %struct._IO_FILE*, %struct._IO_FILE** @stderr, align 8, !dbg !250
  %call11 = call i32 (%struct._IO_FILE*, i8*, ...) @fprintf(%struct._IO_FILE* %12, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @.str.9, i32 0, i32 0)), !dbg !252
  store i32 0, i32* %retval, align 4, !dbg !253
  br label %return, !dbg !253

if.end12:                                         ; preds = %for.end
  %13 = load %struct.passwd*, %struct.passwd** %pw, align 8, !dbg !254
  %14 = bitcast %struct.passwd* %newpw to i8*, !dbg !255
  %15 = bitcast %struct.passwd* %13 to i8*, !dbg !255
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %14, i8* %15, i64 24, i32 8, i1 false), !dbg !255
  %arrayidx13 = getelementptr inbounds [3 x i8*], [3 x i8*]* %fields, i64 0, i64 1, !dbg !256
  %16 = load i8*, i8** %arrayidx13, align 8, !dbg !256
  %pw_dir = getelementptr inbounds %struct.passwd, %struct.passwd* %newpw, i32 0, i32 1, !dbg !257
  store i8* %16, i8** %pw_dir, align 8, !dbg !258
  %arrayidx14 = getelementptr inbounds [3 x i8*], [3 x i8*]* %fields, i64 0, i64 2, !dbg !259
  %17 = load i8*, i8** %arrayidx14, align 16, !dbg !259
  %pw_shell = getelementptr inbounds %struct.passwd, %struct.passwd* %newpw, i32 0, i32 2, !dbg !260
  store i8* %17, i8** %pw_shell, align 8, !dbg !261
  call void @pw_update(%struct.passwd* %newpw), !dbg !262
  store i32 0, i32* %retval, align 4, !dbg !263
  br label %return, !dbg !263

return:                                           ; preds = %if.end12, %if.then10
  %18 = load i32, i32* %retval, align 4, !dbg !264
  ret i32 %18, !dbg !264
}

; Function Attrs: nounwind
declare void @llvm.var.annotation(i8*, i8*, i8*, i32) #5

declare i32 @printf(i8*, ...) #2

; Function Attrs: noreturn nounwind
declare void @exit(i32) #6

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i32, i1) #7

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { nounwind readonly "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { nounwind }
attributes #6 = { noreturn nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #7 = { argmemonly nounwind }
attributes #8 = { nounwind readonly }
attributes #9 = { noreturn nounwind }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!13, !14}
!llvm.ident = !{!15}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 4.0.0 (tags/RELEASE_400/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, retainedTypes: !3)
!1 = !DIFile(filename: "func_partition.c", directory: "/home/james/code/test/prog_partition")
!2 = !{}
!3 = !{!4, !5}
!4 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!5 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !6, size: 64)
!6 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "passwd", file: !1, line: 5, size: 192, elements: !7)
!7 = !{!8, !11, !12}
!8 = !DIDerivedType(tag: DW_TAG_member, name: "pw_id", scope: !6, file: !1, line: 6, baseType: !9, size: 64)
!9 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !10, size: 64)
!10 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!11 = !DIDerivedType(tag: DW_TAG_member, name: "pw_dir", scope: !6, file: !1, line: 7, baseType: !9, size: 64, offset: 64)
!12 = !DIDerivedType(tag: DW_TAG_member, name: "pw_shell", scope: !6, file: !1, line: 8, baseType: !9, size: 64, offset: 128)
!13 = !{i32 2, !"Dwarf Version", i32 4}
!14 = !{i32 2, !"Debug Info Version", i32 3}
!15 = !{!"clang version 4.0.0 (tags/RELEASE_400/final)"}
!16 = distinct !DISubprogram(name: "pw_locate", scope: !1, file: !1, line: 11, type: !17, isLocal: false, isDefinition: true, scopeLine: 11, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!17 = !DISubroutineType(types: !18)
!18 = !{!5, !19}
!19 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !20, size: 64)
!20 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !10)
!21 = !DILocalVariable(name: "id", arg: 1, scope: !16, file: !1, line: 11, type: !19)
!22 = !DIExpression()
!23 = !DILocation(line: 11, column: 38, scope: !16)
!24 = !DILocalVariable(name: "fp", scope: !16, file: !1, line: 12, type: !25)
!25 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !26, size: 64)
!26 = !DIDerivedType(tag: DW_TAG_typedef, name: "FILE", file: !27, line: 48, baseType: !28)
!27 = !DIFile(filename: "/usr/include/stdio.h", directory: "/home/james/code/test/prog_partition")
!28 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "_IO_FILE", file: !29, line: 241, size: 1728, elements: !30)
!29 = !DIFile(filename: "/usr/include/libio.h", directory: "/home/james/code/test/prog_partition")
!30 = !{!31, !33, !34, !35, !36, !37, !38, !39, !40, !41, !42, !43, !44, !52, !53, !54, !55, !59, !61, !63, !67, !70, !72, !73, !74, !75, !76, !80, !81}
!31 = !DIDerivedType(tag: DW_TAG_member, name: "_flags", scope: !28, file: !29, line: 242, baseType: !32, size: 32)
!32 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!33 = !DIDerivedType(tag: DW_TAG_member, name: "_IO_read_ptr", scope: !28, file: !29, line: 247, baseType: !9, size: 64, offset: 64)
!34 = !DIDerivedType(tag: DW_TAG_member, name: "_IO_read_end", scope: !28, file: !29, line: 248, baseType: !9, size: 64, offset: 128)
!35 = !DIDerivedType(tag: DW_TAG_member, name: "_IO_read_base", scope: !28, file: !29, line: 249, baseType: !9, size: 64, offset: 192)
!36 = !DIDerivedType(tag: DW_TAG_member, name: "_IO_write_base", scope: !28, file: !29, line: 250, baseType: !9, size: 64, offset: 256)
!37 = !DIDerivedType(tag: DW_TAG_member, name: "_IO_write_ptr", scope: !28, file: !29, line: 251, baseType: !9, size: 64, offset: 320)
!38 = !DIDerivedType(tag: DW_TAG_member, name: "_IO_write_end", scope: !28, file: !29, line: 252, baseType: !9, size: 64, offset: 384)
!39 = !DIDerivedType(tag: DW_TAG_member, name: "_IO_buf_base", scope: !28, file: !29, line: 253, baseType: !9, size: 64, offset: 448)
!40 = !DIDerivedType(tag: DW_TAG_member, name: "_IO_buf_end", scope: !28, file: !29, line: 254, baseType: !9, size: 64, offset: 512)
!41 = !DIDerivedType(tag: DW_TAG_member, name: "_IO_save_base", scope: !28, file: !29, line: 256, baseType: !9, size: 64, offset: 576)
!42 = !DIDerivedType(tag: DW_TAG_member, name: "_IO_backup_base", scope: !28, file: !29, line: 257, baseType: !9, size: 64, offset: 640)
!43 = !DIDerivedType(tag: DW_TAG_member, name: "_IO_save_end", scope: !28, file: !29, line: 258, baseType: !9, size: 64, offset: 704)
!44 = !DIDerivedType(tag: DW_TAG_member, name: "_markers", scope: !28, file: !29, line: 260, baseType: !45, size: 64, offset: 768)
!45 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !46, size: 64)
!46 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "_IO_marker", file: !29, line: 156, size: 192, elements: !47)
!47 = !{!48, !49, !51}
!48 = !DIDerivedType(tag: DW_TAG_member, name: "_next", scope: !46, file: !29, line: 157, baseType: !45, size: 64)
!49 = !DIDerivedType(tag: DW_TAG_member, name: "_sbuf", scope: !46, file: !29, line: 158, baseType: !50, size: 64, offset: 64)
!50 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !28, size: 64)
!51 = !DIDerivedType(tag: DW_TAG_member, name: "_pos", scope: !46, file: !29, line: 162, baseType: !32, size: 32, offset: 128)
!52 = !DIDerivedType(tag: DW_TAG_member, name: "_chain", scope: !28, file: !29, line: 262, baseType: !50, size: 64, offset: 832)
!53 = !DIDerivedType(tag: DW_TAG_member, name: "_fileno", scope: !28, file: !29, line: 264, baseType: !32, size: 32, offset: 896)
!54 = !DIDerivedType(tag: DW_TAG_member, name: "_flags2", scope: !28, file: !29, line: 268, baseType: !32, size: 32, offset: 928)
!55 = !DIDerivedType(tag: DW_TAG_member, name: "_old_offset", scope: !28, file: !29, line: 270, baseType: !56, size: 64, offset: 960)
!56 = !DIDerivedType(tag: DW_TAG_typedef, name: "__off_t", file: !57, line: 131, baseType: !58)
!57 = !DIFile(filename: "/usr/include/x86_64-linux-gnu/bits/types.h", directory: "/home/james/code/test/prog_partition")
!58 = !DIBasicType(name: "long int", size: 64, encoding: DW_ATE_signed)
!59 = !DIDerivedType(tag: DW_TAG_member, name: "_cur_column", scope: !28, file: !29, line: 274, baseType: !60, size: 16, offset: 1024)
!60 = !DIBasicType(name: "unsigned short", size: 16, encoding: DW_ATE_unsigned)
!61 = !DIDerivedType(tag: DW_TAG_member, name: "_vtable_offset", scope: !28, file: !29, line: 275, baseType: !62, size: 8, offset: 1040)
!62 = !DIBasicType(name: "signed char", size: 8, encoding: DW_ATE_signed_char)
!63 = !DIDerivedType(tag: DW_TAG_member, name: "_shortbuf", scope: !28, file: !29, line: 276, baseType: !64, size: 8, offset: 1048)
!64 = !DICompositeType(tag: DW_TAG_array_type, baseType: !10, size: 8, elements: !65)
!65 = !{!66}
!66 = !DISubrange(count: 1)
!67 = !DIDerivedType(tag: DW_TAG_member, name: "_lock", scope: !28, file: !29, line: 280, baseType: !68, size: 64, offset: 1088)
!68 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !69, size: 64)
!69 = !DIDerivedType(tag: DW_TAG_typedef, name: "_IO_lock_t", file: !29, line: 150, baseType: null)
!70 = !DIDerivedType(tag: DW_TAG_member, name: "_offset", scope: !28, file: !29, line: 289, baseType: !71, size: 64, offset: 1152)
!71 = !DIDerivedType(tag: DW_TAG_typedef, name: "__off64_t", file: !57, line: 132, baseType: !58)
!72 = !DIDerivedType(tag: DW_TAG_member, name: "__pad1", scope: !28, file: !29, line: 297, baseType: !4, size: 64, offset: 1216)
!73 = !DIDerivedType(tag: DW_TAG_member, name: "__pad2", scope: !28, file: !29, line: 298, baseType: !4, size: 64, offset: 1280)
!74 = !DIDerivedType(tag: DW_TAG_member, name: "__pad3", scope: !28, file: !29, line: 299, baseType: !4, size: 64, offset: 1344)
!75 = !DIDerivedType(tag: DW_TAG_member, name: "__pad4", scope: !28, file: !29, line: 300, baseType: !4, size: 64, offset: 1408)
!76 = !DIDerivedType(tag: DW_TAG_member, name: "__pad5", scope: !28, file: !29, line: 302, baseType: !77, size: 64, offset: 1472)
!77 = !DIDerivedType(tag: DW_TAG_typedef, name: "size_t", file: !78, line: 62, baseType: !79)
!78 = !DIFile(filename: "/home/james/code/llvm-4.0.0.src/build/bin/../lib/clang/4.0.0/include/stddef.h", directory: "/home/james/code/test/prog_partition")
!79 = !DIBasicType(name: "long unsigned int", size: 64, encoding: DW_ATE_unsigned)
!80 = !DIDerivedType(tag: DW_TAG_member, name: "_mode", scope: !28, file: !29, line: 303, baseType: !32, size: 32, offset: 1536)
!81 = !DIDerivedType(tag: DW_TAG_member, name: "_unused2", scope: !28, file: !29, line: 305, baseType: !82, size: 160, offset: 1568)
!82 = !DICompositeType(tag: DW_TAG_array_type, baseType: !10, size: 160, elements: !83)
!83 = !{!84}
!84 = !DISubrange(count: 20)
!85 = !DILocation(line: 12, column: 8, scope: !16)
!86 = !DILocation(line: 12, column: 13, scope: !16)
!87 = !DILocation(line: 13, column: 6, scope: !88)
!88 = distinct !DILexicalBlock(scope: !16, file: !1, line: 13, column: 6)
!89 = !DILocation(line: 13, column: 9, scope: !88)
!90 = !DILocation(line: 13, column: 6, scope: !16)
!91 = !DILocation(line: 14, column: 11, scope: !92)
!92 = distinct !DILexicalBlock(scope: !88, file: !1, line: 13, column: 18)
!93 = !DILocation(line: 14, column: 3, scope: !92)
!94 = !DILocation(line: 15, column: 3, scope: !92)
!95 = !DILocalVariable(name: "buf", scope: !16, file: !1, line: 17, type: !96)
!96 = !DICompositeType(tag: DW_TAG_array_type, baseType: !10, size: 2048, elements: !97)
!97 = !{!98}
!98 = !DISubrange(count: 256)
!99 = !DILocation(line: 17, column: 7, scope: !16)
!100 = !DILocalVariable(name: "pw", scope: !16, file: !1, line: 18, type: !5)
!101 = !DILocation(line: 18, column: 17, scope: !16)
!102 = !DILocation(line: 18, column: 39, scope: !16)
!103 = !DILocation(line: 18, column: 22, scope: !16)
!104 = !DILocation(line: 19, column: 6, scope: !105)
!105 = distinct !DILexicalBlock(scope: !16, file: !1, line: 19, column: 6)
!106 = !DILocation(line: 19, column: 9, scope: !105)
!107 = !DILocation(line: 19, column: 6, scope: !16)
!108 = !DILocation(line: 20, column: 11, scope: !109)
!109 = distinct !DILexicalBlock(scope: !105, file: !1, line: 19, column: 18)
!110 = !DILocation(line: 20, column: 3, scope: !109)
!111 = !DILocation(line: 21, column: 3, scope: !109)
!112 = !DILocation(line: 23, column: 2, scope: !16)
!113 = !DILocation(line: 23, column: 15, scope: !114)
!114 = !DILexicalBlockFile(scope: !16, file: !1, discriminator: 1)
!115 = !DILocation(line: 23, column: 10, scope: !114)
!116 = !DILocation(line: 23, column: 9, scope: !114)
!117 = !DILocation(line: 23, column: 2, scope: !114)
!118 = !DILocation(line: 24, column: 13, scope: !119)
!119 = distinct !DILexicalBlock(scope: !120, file: !1, line: 24, column: 7)
!120 = distinct !DILexicalBlock(scope: !16, file: !1, line: 23, column: 20)
!121 = !DILocation(line: 24, column: 31, scope: !119)
!122 = !DILocation(line: 24, column: 7, scope: !119)
!123 = !DILocation(line: 24, column: 35, scope: !119)
!124 = !DILocation(line: 24, column: 7, scope: !120)
!125 = !DILocation(line: 25, column: 12, scope: !126)
!126 = distinct !DILexicalBlock(scope: !119, file: !1, line: 24, column: 44)
!127 = !DILocation(line: 25, column: 4, scope: !126)
!128 = !DILocation(line: 26, column: 4, scope: !126)
!129 = !DILocalVariable(name: "p", scope: !120, file: !1, line: 28, type: !9)
!130 = !DILocation(line: 28, column: 9, scope: !120)
!131 = !DILocation(line: 28, column: 20, scope: !120)
!132 = !DILocation(line: 28, column: 13, scope: !120)
!133 = !DILocation(line: 29, column: 7, scope: !134)
!134 = distinct !DILexicalBlock(scope: !120, file: !1, line: 29, column: 7)
!135 = !DILocation(line: 29, column: 7, scope: !120)
!136 = !DILocation(line: 30, column: 23, scope: !134)
!137 = !DILocation(line: 30, column: 16, scope: !134)
!138 = !DILocation(line: 30, column: 4, scope: !134)
!139 = !DILocation(line: 30, column: 8, scope: !134)
!140 = !DILocation(line: 30, column: 14, scope: !134)
!141 = !DILocation(line: 32, column: 4, scope: !134)
!142 = distinct !{!142, !112, !143}
!143 = !DILocation(line: 46, column: 2, scope: !16)
!144 = !DILocation(line: 33, column: 14, scope: !145)
!145 = distinct !DILexicalBlock(scope: !120, file: !1, line: 33, column: 7)
!146 = !DILocation(line: 33, column: 18, scope: !145)
!147 = !DILocation(line: 33, column: 25, scope: !145)
!148 = !DILocation(line: 33, column: 7, scope: !145)
!149 = !DILocation(line: 33, column: 29, scope: !145)
!150 = !DILocation(line: 33, column: 7, scope: !120)
!151 = !DILocation(line: 34, column: 8, scope: !152)
!152 = distinct !DILexicalBlock(scope: !145, file: !1, line: 33, column: 35)
!153 = !DILocation(line: 34, column: 6, scope: !152)
!154 = !DILocation(line: 35, column: 8, scope: !155)
!155 = distinct !DILexicalBlock(scope: !152, file: !1, line: 35, column: 8)
!156 = !DILocation(line: 35, column: 8, scope: !152)
!157 = !DILocation(line: 36, column: 25, scope: !155)
!158 = !DILocation(line: 36, column: 18, scope: !155)
!159 = !DILocation(line: 36, column: 5, scope: !155)
!160 = !DILocation(line: 36, column: 9, scope: !155)
!161 = !DILocation(line: 36, column: 16, scope: !155)
!162 = !DILocation(line: 38, column: 5, scope: !155)
!163 = !DILocation(line: 39, column: 8, scope: !152)
!164 = !DILocation(line: 39, column: 6, scope: !152)
!165 = !DILocation(line: 40, column: 8, scope: !166)
!166 = distinct !DILexicalBlock(scope: !152, file: !1, line: 40, column: 8)
!167 = !DILocation(line: 40, column: 8, scope: !152)
!168 = !DILocation(line: 41, column: 27, scope: !166)
!169 = !DILocation(line: 41, column: 20, scope: !166)
!170 = !DILocation(line: 41, column: 5, scope: !166)
!171 = !DILocation(line: 41, column: 9, scope: !166)
!172 = !DILocation(line: 41, column: 18, scope: !166)
!173 = !DILocation(line: 43, column: 5, scope: !166)
!174 = !DILocation(line: 44, column: 11, scope: !152)
!175 = !DILocation(line: 44, column: 4, scope: !152)
!176 = !DILocation(line: 23, column: 2, scope: !177)
!177 = !DILexicalBlockFile(scope: !16, file: !1, discriminator: 2)
!178 = !DILocation(line: 47, column: 7, scope: !16)
!179 = !DILocation(line: 47, column: 2, scope: !16)
!180 = !DILocation(line: 48, column: 2, scope: !16)
!181 = !DILocation(line: 49, column: 1, scope: !16)
!182 = distinct !DISubprogram(name: "pw_update", scope: !1, file: !1, line: 51, type: !183, isLocal: false, isDefinition: true, scopeLine: 51, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!183 = !DISubroutineType(types: !184)
!184 = !{null, !185}
!185 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !186, size: 64)
!186 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !6)
!187 = !DILocalVariable(name: "pw", arg: 1, scope: !182, file: !1, line: 51, type: !185)
!188 = !DILocation(line: 51, column: 37, scope: !182)
!189 = !DILocation(line: 52, column: 1, scope: !182)
!190 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 54, type: !191, isLocal: false, isDefinition: true, scopeLine: 54, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!191 = !DISubroutineType(types: !192)
!192 = !{!32, !32, !193}
!193 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !9, size: 64)
!194 = !DILocalVariable(name: "argc", arg: 1, scope: !190, file: !1, line: 54, type: !32)
!195 = !DILocation(line: 54, column: 14, scope: !190)
!196 = !DILocalVariable(name: "argv", arg: 2, scope: !190, file: !1, line: 54, type: !193)
!197 = !DILocation(line: 54, column: 26, scope: !190)
!198 = !DILocalVariable(name: "pw", scope: !190, file: !1, line: 55, type: !185)
!199 = !DILocation(line: 55, column: 62, scope: !190)
!200 = !DILocation(line: 55, column: 2, scope: !190)
!201 = !DILocalVariable(name: "newpw", scope: !190, file: !1, line: 56, type: !6)
!202 = !DILocation(line: 56, column: 16, scope: !190)
!203 = !DILocalVariable(name: "fields", scope: !190, file: !1, line: 57, type: !204)
!204 = !DICompositeType(tag: DW_TAG_array_type, baseType: !9, size: 192, elements: !205)
!205 = !{!206}
!206 = !DISubrange(count: 3)
!207 = !DILocation(line: 57, column: 8, scope: !190)
!208 = !DILocation(line: 59, column: 6, scope: !209)
!209 = distinct !DILexicalBlock(scope: !190, file: !1, line: 59, column: 6)
!210 = !DILocation(line: 59, column: 11, scope: !209)
!211 = !DILocation(line: 59, column: 6, scope: !190)
!212 = !DILocation(line: 60, column: 44, scope: !213)
!213 = distinct !DILexicalBlock(scope: !209, file: !1, line: 59, column: 17)
!214 = !DILocation(line: 60, column: 3, scope: !213)
!215 = !DILocation(line: 61, column: 3, scope: !213)
!216 = !DILocalVariable(name: "i", scope: !190, file: !1, line: 64, type: !32)
!217 = !DILocation(line: 64, column: 6, scope: !190)
!218 = !DILocation(line: 65, column: 9, scope: !219)
!219 = distinct !DILexicalBlock(scope: !190, file: !1, line: 65, column: 2)
!220 = !DILocation(line: 65, column: 7, scope: !219)
!221 = !DILocation(line: 65, column: 14, scope: !222)
!222 = !DILexicalBlockFile(scope: !223, file: !1, discriminator: 1)
!223 = distinct !DILexicalBlock(scope: !219, file: !1, line: 65, column: 2)
!224 = !DILocation(line: 65, column: 18, scope: !222)
!225 = !DILocation(line: 65, column: 16, scope: !222)
!226 = !DILocation(line: 65, column: 2, scope: !227)
!227 = !DILexicalBlockFile(scope: !219, file: !1, discriminator: 1)
!228 = !DILocation(line: 66, column: 26, scope: !229)
!229 = distinct !DILexicalBlock(scope: !223, file: !1, line: 65, column: 29)
!230 = !DILocation(line: 66, column: 31, scope: !229)
!231 = !DILocation(line: 66, column: 19, scope: !229)
!232 = !DILocation(line: 66, column: 10, scope: !229)
!233 = !DILocation(line: 66, column: 12, scope: !229)
!234 = !DILocation(line: 66, column: 3, scope: !229)
!235 = !DILocation(line: 66, column: 17, scope: !229)
!236 = !DILocation(line: 67, column: 2, scope: !229)
!237 = !DILocation(line: 65, column: 25, scope: !238)
!238 = !DILexicalBlockFile(scope: !223, file: !1, discriminator: 2)
!239 = !DILocation(line: 65, column: 2, scope: !238)
!240 = distinct !{!240, !241, !242}
!241 = !DILocation(line: 65, column: 2, scope: !219)
!242 = !DILocation(line: 67, column: 2, scope: !219)
!243 = !DILocation(line: 68, column: 17, scope: !190)
!244 = !DILocation(line: 68, column: 7, scope: !190)
!245 = !DILocation(line: 68, column: 5, scope: !190)
!246 = !DILocation(line: 69, column: 6, scope: !247)
!247 = distinct !DILexicalBlock(scope: !190, file: !1, line: 69, column: 6)
!248 = !DILocation(line: 69, column: 9, scope: !247)
!249 = !DILocation(line: 69, column: 6, scope: !190)
!250 = !DILocation(line: 70, column: 11, scope: !251)
!251 = distinct !DILexicalBlock(scope: !247, file: !1, line: 69, column: 18)
!252 = !DILocation(line: 70, column: 3, scope: !251)
!253 = !DILocation(line: 71, column: 3, scope: !251)
!254 = !DILocation(line: 73, column: 11, scope: !190)
!255 = !DILocation(line: 73, column: 10, scope: !190)
!256 = !DILocation(line: 74, column: 17, scope: !190)
!257 = !DILocation(line: 74, column: 8, scope: !190)
!258 = !DILocation(line: 74, column: 15, scope: !190)
!259 = !DILocation(line: 75, column: 19, scope: !190)
!260 = !DILocation(line: 75, column: 8, scope: !190)
!261 = !DILocation(line: 75, column: 17, scope: !190)
!262 = !DILocation(line: 76, column: 2, scope: !190)
!263 = !DILocation(line: 77, column: 2, scope: !190)
!264 = !DILocation(line: 78, column: 1, scope: !190)
