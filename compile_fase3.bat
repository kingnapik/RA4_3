@echo off
echo === Compilando Analisador Completo (Fases 1, 2 3 e 4) ===
echo.

g++ -std=c++17 -Wall -O2 -c gramatica.cpp -o gramatica.o
if errorlevel 1 goto error

g++ -std=c++17 -Wall -O2 -c parser.cpp -o parser.o
if errorlevel 1 goto error

g++ -std=c++17 -Wall -O2 -c arvore.cpp -o arvore.o
if errorlevel 1 goto error

g++ -std=c++17 -Wall -O2 -c leitor.cpp -o leitor.o
if errorlevel 1 goto error

g++ -std=c++17 -Wall -O2 -c tabela_simbolos.cpp -o tabela_simbolos.o
if errorlevel 1 goto error

g++ -std=c++17 -Wall -O2 -c semantico_driver.cpp -o semantico_driver.o
if errorlevel 1 goto error

g++ -std=c++17 -Wall -O2 -c semantico_comandos.cpp -o semantico_comandos.o
if errorlevel 1 goto error

g++ -std=c++17 -Wall -O2 -c semantico_inferencia.cpp -o semantico_inferencia.o
if errorlevel 1 goto error

g++ -std=c++17 -Wall -O2 -c semantico_saida.cpp -o semantico_saida.o
if errorlevel 1 goto error

g++ -std=c++17 -Wall -O2 -c tac.cpp -o tac.o
if errorlevel 1 goto error

g++ -std=c++17 -Wall -O2 -c main.cpp -o main.o
if errorlevel 1 goto error

g++ -std=c++17 -Wall -O2 -o AnalisadorCompleto.exe main.o gramatica.o parser.o arvore.o leitor.o tabela_simbolos.o semantico_saida.o semantico_inferencia.o semantico_comandos.o semantico_driver.o tac.o
if errorlevel 1 goto error

echo.
echo === Compilacao concluida com sucesso! ===
echo.
echo Execute com:
echo   .\AnalisadorCompleto.exe arquivo.txt
echo.
goto end

:error
echo.
echo === ERRO na compilacao ===
pause
exit /b 1

:end
