namespace Filesystem
{
    class File {        
        function open(copy path: String)
        {
            /* 
                Ni idea de cómo se maneja esto específicamente, implementándolo aprenderé 
                Usa una librería, una api, una call ¿Qué podré saber? Como quiero que interopere hasta con una tostadora hasta podría usar libc y ya 😃
                Pues con estas macros podría, en teoría, importar condicionalmente y todo, solo es texto. En efecto la kword import ya está y me arma el arbol de dependencias con variables de entorno, archivos locales y todo.
                ¿Vale la pena empezar al reves? ¿Quién se preocupa de las dependencias?
                Cada vez avanso, aunque sea un poco, ya tengo una parte importantísima ahora que lo pienso 😂🙏
                ¿Cuando es buen momento para comenzar a definir el ecosistema?
            */
            @if ( CONFIG.OS == "linux" ) { ... }
            ...
        }

        function isOpen(): bool { ... }
    }
}

/*
    La triste historia del pollo que nunca se asó: 😔🔥🎩🍷
*/