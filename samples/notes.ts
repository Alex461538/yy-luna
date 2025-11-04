
/* 
 
@implements
@
@
@
@
@

mmmmmmmmmm
*/

/* Pillelo mi bro, se añaden entre si y multiplican por escalar */
/* type Term = @adds_with<self> and @muls_with<float>; */

/* 
    Use decorators for adding extra metadata to anyhing 
*/
class Vector {
    x: float;
    y: float;

    constructor()
    {
        this.x = 0;
        this.y = 0;
    }

    /* opr+()
    {
        return new 
    } */
}

/*

Vea pues mi perro, usted si ha pillado las tripletas de compilación?
En cuanto a mí, parecían buenas al principio, pero pueden traer varios problemas.

Entonces aquí le voy a pelar una nueva forma de ver esto, como tripletas, pero más separadas:

    - isa: el set de instrucciones, mejor dicho, lo que le dicen arquitectura
    - pack: esto es el formato de ejecutable, o empaquetado, usted verá si quiere 
        poner instrucciones de x86 en una rom de nes, ese es su problema bro, si el libre albedrío existiera.
        O si quiere ignorar la pasada de ensamblaje y documentar todo.
    - osapi: Para las librerías estandar, no es propiamente parte esencial del proceso, 
        pero ayuda a compilar condicionalmente y elegir api's, y syscalls y todo eso.

    Aquí se puede pillar opciones comunes con esto:
        -isa x64 -pack elf -osapi linux ; Pa linux, y luego no funciona
        -isa x64 -pack exe -osapi win32 ; Pa la partición de windows 11 desgraciado tan lento
        -isa arm8 -pack dex??? -osapi android-?? - ; Pa mi celular panela, de verdad de android no se nada, 
            como que eso requiere todo un toolchain separado pa empaquetar luego en apk, wtf ni se en qué versión vá android :D
        -isa none -osapi none -pack pdf-latex ; Alguna documentación, latex haría el trabajo de diseño que no me sale bien.
    
    Y la ABI qué?
    Como tengo todo ahora, en el momento de hacer ffi e importar y exportar cosas
    deberías poner eso manualmente con decoradores raros y simples. Así no quedan dudas de nada.
    Si algo de esto no tiene sentido para -isa, o -pack no me rompo la cabeza, y digo en un error que no me la sé.

    @calling< sys_v, ms64, cc65... > Solo conozco esas tres :D
    @fields< backwards >
    @alignment< packed, natural, <num_bytes>... >
    @mangle< yy, msvc, gcc_for_AMOG_OS, gcc_for_RED_STAR_OS... ...infinity >
    ...?

    Pero en el caso de que no uses nada de eso... ¿Qué importa, si el programa funciona perfecto internamente?
    Ahí me robaré la abi más facil que encuentre de alguna versión de gcc, ni idea.

    Y bueno, con osapi se selecciona la que es pal sistema... creo.
*/