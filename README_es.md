<div align="center">

![WIP](https://img.shields.io/badge/work%20in%20progress-yellow?style=for-the-badge)
![System & Kernel](https://img.shields.io/badge/System-brown?style=for-the-badge)
![Terminal](https://img.shields.io/badge/Terminal-PTY-blue?style=for-the-badge)
![C Language](https://img.shields.io/badge/Language-C-red?style=for-the-badge)

*Una reimplementación del comando script de Unix*

</div>

<div align="center">
	<img src="/images/ft_script.jpg">
</div>

# ft_script

[README in English](README.md)

`ft_script` es una reimplementacion del comando Unix `script`.

Registra sesiones de terminal usando una PTY y permite guardar:
- salida (por defecto)
- entrada (opcional)
- metadatos de tiempo para replay/analisis

El proyecto esta hecho en C con APIs Unix de bajo nivel y parseo minimo de opciones.

## Instalacion

```bash
git clone https://github.com/Kobayashi82/ft_script.git
cd ft_script
make
```

## Uso

```bash
./ft_script [options] [file]
```

Si no se indica archivo, la salida se guarda en `typescript`.

### Uso rapido

```bash
# Sesion interactiva, archivo por defecto: typescript
./ft_script

# Sesion interactiva con archivo personalizado
./ft_script sesion.log

# Ejecutar comando y salir
./ft_script -c "ls -la"
```

### Opciones
| Opcion         | Descripcion                                           |
|----------------|-------------------------------------------------------|
| `-I <file>`    | Guarda stdin en archivo                               |
| `-O <file>`    | Guarda stdout en archivo (comportamiento por defecto) |
| `-B <file>`    | Guarda stdin y stdout en el mismo archivo             |
| `-T <file>`    | Guarda informacion de timing en archivo               |
| `-m <name>`    | Fuerza formato: `classic` o `advanced`                |
| `-a`           | Anade al final del archivo en vez de truncar          |
| `-c <command>` | Ejecuta un comando en lugar de shell interactiva      |
| `-e`           | Devuelve el codigo de salida del proceso hijo         |
| `-f`           | Fuerza flush en cada escritura                        |
| `-E <when>`    | Echo de entrada: `auto`, `always` o `never`           |
| `-o <size>`    | Termina cuando los archivos de salida superan tamano  |
| `-q`           | Modo silencioso                                       |
| `-h`           | Muestra ayuda                                         |
| `-V`           | Muestra version                                       |
|

### Formatos de log

- `advanced` (por defecto): formato moderno con metadatos de tiempo mas ricos
- `classic`: compatibilidad con formato clasico

### Ejemplos

```bash
# Guardar entrada y salida en un solo archivo
./ft_script -B sesion.log

# Guardar salida + timing en formato clasico
./ft_script -O out.log -T timing.log -m classic

# Anadir y hacer flush inmediato en cada escritura
./ft_script -a -f -O out.log

# Devolver codigo de salida del comando
./ft_script -e -c "grep hello missing_file"
```

### Notas

- Solo se soportan flags simples (`-a`, no opciones agrupadas como `-abc`).
- Esta orientado principalmente a sesiones interactivas de terminal.
- Para detalles completos y limitaciones, revisa `./ft_script -h`.

## Licencia

Este proyecto esta licenciado bajo la WTFPL – [Do What the Fuck You Want to Public License](http://www.wtfpl.net/about/).

---

<div align="center">

**📂 Desarrollado como parte del curriculum de 42 School 📂**

*"Everything you typed, faithfully recorded... whether you meant to or not"*

</div>