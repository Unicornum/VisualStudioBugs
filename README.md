
# Visual Studio Bugs

Проекты, воспроизводящие ошибки в работе Visual Studio.

## Актуально (v15.9.12)

### CrusherTwoClasses

Падение программы во время работы при использовании классов с одним и тем же 
именем в разных .cpp файлах.

### DisableAllWarnings

Следующая директива должна отключать все предупреждения при сборке проекта:

#pragma warning(push, 0)

### PragmaMessageFromTemplate

Следующая директива выводит предупреждение игнорируя русские буквы при 
использовании внутри шаблонной функции.

__pragma(message("Pragma message: шаблонная функция 2"));

### ErrorGetRecipeFile

Если в решении есть Android проект, которому установлена зависимость от Windows
проекта, то сборка в Visual Studio работает нормально, но сборка скриптом
при помощи MSBuild (для воспроизведения запускать ErrorGetRecipeFile.cmd) 
завершается с ошибкой вида 

...\Simple.vcxproj : error MSB4057: в проекте нет целевого объекта "GetRecipeFile".

## Найдено обходное решение

### BuildWithEarlierLibrary

Сборка проекта с библиотекой, собранной более ранней версией компилятора.

Проявляется как ошибка линковщика, для обхода проблемы добавить в командную строку линковщика параметр

/d2:-AllowCompatibleILVersions

### OpenObjFile

Проблема: после открытия .obj файла 3D модели и ее сохранении она сохраняется 
в формате 'utf8 без сигнатуры' и в дальнейшем открывается как текстовый файл.

Воспроизведение:
1. Двойной клик по файлу Boulder.obj -> он открывается в 3D редакторе.
2. Выделяем объект, в свойствах меняем его позицию, сохраняем, закрываем окно.
3. Двойной клик по файлу Boulder.obj -> он открывается в текстовом редакторе.
4. Сохраняем файл как 'utf8 с сигнатурой', закрываем окно.
5. Двойной клик по файлу Boulder.obj -> он открывается в 3D редакторе.

## Исправлено

...
