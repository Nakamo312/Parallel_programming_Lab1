@echo off
setlocal enabledelayedexpansion

:: Параметры по умолчанию
set DEFAULT_SIZES=100 200 300 400 500 600 700 800 900 1000 2000
set DEFAULT_CSV=results.csv
set DEFAULT_PLOT=performance_plot.png

:: Парсинг аргументов командной строки
set "sizes=%DEFAULT_SIZES%"
set "csv_file=%DEFAULT_CSV%"
set "plot_file=%DEFAULT_PLOT%"

:parse_args
if "%~1"=="" goto end_parse
if "%~1"=="--sizes" (
    set "sizes=%~2"
    shift
    shift
    goto parse_args
)
if "%~1"=="--csv" (
    set "csv_file=%~2"
    shift
    shift
    goto parse_args
)
if "%~1"=="--plot" (
    set "plot_file=%~2"
    shift
    shift
    goto parse_args
)
shift
goto parse_args

:end_parse

:: Инициализация окружения
if not exist venv (
    python -m venv venv
    if errorlevel 1 exit /b 1
)

call venv\Scripts\activate.bat
if errorlevel 1 exit /b 1

python -c "import numpy" 2>nul
python -c "import matplotlib" 2>nul
python -c "import pandas" 2>nul
python -c "import seaborn" 2>nul
if %errorlevel% neq 0 (
    if exist requirements.txt (
        pip install -r requirements.txt
    ) else (
        pip install numpy matplotlib pandas seaborn
    )
    if errorlevel 1 exit /b 1
)

:: Основной цикл выполнения
set matrix1=mat1.bin
set matrix2=mat2.bin
set output=result.bin

for %%s in (%sizes%) do (
    echo Processing size %%s x %%s...
    call :main %%s
)

:: Построение графика
echo Building performance plot...
python graph.py --csv "%csv_file%" --output "%plot_file%"
endlocal
exit /b 0

:main
setlocal
set size=%1

echo Generating matrices %matrix1% and %matrix2%...
Parallel_programming__Lab_1.exe generate %size% %size% %size% %size% "%matrix1%" "%matrix2%"

if errorlevel 1 (
    echo Error generating matrices!
    exit /b 1
)

echo Multiplying matrices...
for /F "delims=" %%a in ('Parallel_programming__Lab_1.exe multiply "%matrix1%" "%matrix2%" "%output%" 2^>^&1 1^>NUL') do (
    set "duration=%%a"
)

if errorlevel 1 (
    echo Matrix multiplication failed!
    exit /b 1
)

echo Multiplication Time for size %size% (ms): !duration!

python validate.py "%matrix1%" "%matrix2%" "%output%" --csv "%csv_file%" --duration !duration!

if errorlevel 1 (
    echo Validation failed!
    exit /b 1
)

del "%matrix1%"
del "%matrix2%"
del "%output%"

endlocal
exit /b 0