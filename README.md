# Sistema avanzado de analisis

## 👥 GRUPO 1 - Diseño de Algoritmos

### 👨‍💻 Integrantes del Equipo

| Nombre | Carrera | Email | Responsabilidades |
|--------|---------|-------|-------------------|
| **Fernando Garay** | Ingeniería en Computación | | Algoritmos de Búsqueda, Documentación |
| **Francisco Miranda** | Ingeniería en Computación |  | Estructuras de Datos, Testing |
| **Ignacio Contreras** | Ingeniería en Computación |  | Preprocesamiento, Análisis de Rendimiento |

---

## 📋 Descripción del Proyecto

Sistema completo de análisis de documentos que implementa algoritmos avanzados de procesamiento de texto y búsqueda de patrones. El proyecto funciona exclusivamente desde la línea de comandos y está diseñado para indexar, buscar y analizar grandes volúmenes de texto de manera eficiente.

## 🎯 Objetivos

- Implementar algoritmos avanzados de búsqueda de patrones en texto
- Desarrollar estructuras de datos eficientes para indexación
- Analizar empíricamente la complejidad y rendimiento de los algoritmos
- Crear un sistema práctico de procesamiento de documentos
- Implementar técnicas de preprocesamiento para mejorar la eficiencia

## 🔧 Algoritmos Implementados

### 🔍 Algoritmos de Búsqueda de Patrones
1. **Knuth-Morris-Pratt (KMP)**
   - Preprocesamiento de patrones con función de fallos
   - Complejidad: O(n + m)
   - Eficiente para patrones con repeticiones

2. **Shift-And (Bitap)**
   - Algoritmo basado en operaciones de bits
   - Óptimo para patrones cortos (≤64 caracteres)
   - Complejidad: O(n)

3. **Shift-Or**
   - Variante del Shift-And con operaciones OR
   - Comparación directa de rendimiento
   - Implementación compacta

### 🗄️ Estructuras de Datos para Indexación
1. **Tabla Hash para Texto**
   - Funciones hash optimizadas para strings
   - Manejo de colisiones por encadenamiento
   - Análisis de frecuencia de palabras

2. **Trie (Árbol de Prefijos)**
   - Almacenamiento eficiente de diccionarios
   - Búsqueda de prefijos en O(m)
   - Optimización de memoria

### 🔄 Técnicas de Preprocesamiento
1. **Tokenización**
   - División de texto en palabras y n-gramas
   - Manejo de caracteres especiales
   - Soporte para múltiples delimitadores

2. **Normalización**
   - Conversión a minúsculas
   - Eliminación de acentos y diacríticos
   - Normalización de espacios en blanco
