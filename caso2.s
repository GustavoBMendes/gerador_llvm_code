source_filename: 'caso2.calc'
target datalayout = 'e-m:e-i64:64-f80:128-n8:16:32:64-S128'
target triple = 'x86_64-pc-linux-gnu'
@.str = private unnamed_addr constant [6 x i8] c %d %d, align 1

define void @main() %0 {
	%1 = alloca float, align 4
	store float 10.2, float* %1, align 4
	%2 = alloca float, align 4
	store float 10.5, float* %2, align 4
	%3 = alloca float, align 4
	%4 = load float, float* %1, align 4
	%5 = load float, float* %2, align 4
	%6 = add nsw float 10.2, 10.5
	store float 20.7, float* %3, align 4
	%7 = load float, float* %3, align 4
	%8 call float (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), float %7)
	ret void
}

declare i32 @printf(i8*, ...) #1
