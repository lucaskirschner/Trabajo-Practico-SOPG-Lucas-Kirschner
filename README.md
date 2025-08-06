# 🧠 Trabajo Práctico — Servidor TCP Clave-Valor

## 👨‍💻 Autor

- **Lucas Sebastián Kirschner** — `e2409`

---

## 📚 Materia

**Sistemas Operativos de Propósito General**  
**Trabajo Práctico:** _Servidor TCP — Almacenamiento Clave-Valor_

---

## ⚙️ Descripción

Este servidor TCP fue desarrollado como parte del trabajo práctico propuesto en la materia.  
Permite almacenar información en formato texto mediante un esquema clave-valor simple, utilizando archivos como mecanismo de persistencia.

El protocolo de comunicación es **ASCII** sobre **TCP/IP**.

---

## 🛠️ Requisitos

- Linux (probado en Ubuntu)
- Compilador `gcc`
- Herramientas de prueba: `nc` (netcat) o `telnet`

Instalación de herramientas:

```bash
sudo apt install netcat
# o
sudo apt install telnet