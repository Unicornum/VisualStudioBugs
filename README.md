
# Visual Studio Bugs

Проекты, воспроизводящие баги работы Visual Studio.

## Актуально (v15.9.12)

### CrusherTwoClasses

Падение программы во время работы при использовании классов с одним и тем же именем в разных .cpp файлах.

### DisableAllWarnings

Следующая директива должна отключать все предупреждения при сборке проекта:

#pragma warning(push, 0)

### PragmaMessageFromTemplate

Следующая директива выводит предупреждение игнорируя русские буквы при использовании внутри шаблонной функции.

__pragma(message("Pragma message: шаблонная функция 2"));

## Исправлено

...
