@echo off
setlocal enabledelayedexpansion

echo =====================================
echo Compiling source files...

g++ generate_input.cpp -o generate_input.exe
g++ floyd_seq.cpp -o floyd_seq.exe
g++ floyd_mpi.cpp -I"C:/Program Files (x86)/Microsoft SDKs/MPI/Include" -L"C:/Program Files (x86)/Microsoft SDKs/MPI/Lib/x64" -lmsmpi -o floyd_mpi.exe

if errorlevel 1 (
    echo Compilation failed.
    pause
    exit /b 1
)

echo.
echo Generating test input files...
rem Generate test files for 500, 1000, and 1500 nodes
generate_input.exe 500 input_500.txt
generate_input.exe 1000 input_1000.txt
generate_input.exe 1500 input_1500.txt

echo.
echo Running Sequential Version...

floyd_seq.exe input_500.txt
move /Y floyd_seq_result.txt floyd_seq_result_500.txt
move /Y floyd_seq_summary.txt floyd_seq_summary_500.txt

floyd_seq.exe input_1000.txt
move /Y floyd_seq_result.txt floyd_seq_result_1000.txt
move /Y floyd_seq_summary.txt floyd_seq_summary_1000.txt

floyd_seq.exe input_1500.txt
move /Y floyd_seq_result.txt floyd_seq_result_1500.txt
move /Y floyd_seq_summary.txt floyd_seq_summary_1500.txt

echo.
echo Послідовні результати:
echo -----------------------
for %%s in (500 1000 1500) do (
    echo Результати для %%s вершин:
    type floyd_seq_summary_%%s.txt
    echo.
)

echo.
echo Running MPI Version with different process counts...

rem Задаємо кількість процесів для запуску MPI версії
for %%p in (2 4 8) do (
    echo -------------------------------------------
    echo Запуск MPI версії з %%p процесами...

    mpiexec -n %%p floyd_mpi.exe input_500.txt
    move /Y floyd_mpi_result.txt floyd_mpi_result_500_%%p.txt
    move /Y floyd_mpi_summary.txt floyd_mpi_summary_500_%%p.txt

    mpiexec -n %%p floyd_mpi.exe input_1000.txt
    move /Y floyd_mpi_result.txt floyd_mpi_result_1000_%%p.txt
    move /Y floyd_mpi_summary.txt floyd_mpi_summary_1000_%%p.txt

    mpiexec -n %%p floyd_mpi.exe input_1500.txt
    move /Y floyd_mpi_result.txt floyd_mpi_result_1500_%%p.txt
    move /Y floyd_mpi_summary.txt floyd_mpi_summary_1500_%%p.txt

    echo Результати для %%p процесів:
    for %%s in (500 1000 1500) do (
        echo Результати для %%s вершин, %%p процесів:
        type floyd_mpi_summary_%%s_%%p.txt
        echo.
    )
)

echo.
echo Testing completed.
pause
