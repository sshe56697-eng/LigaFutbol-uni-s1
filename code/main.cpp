#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

struct ConfigLiga {
    string nombre;
    int ptsV, ptsE, ptsD;
    vector<string> nombresEquipos;
};

struct Partido {
    string fecha;
    string local, visitante;
    int golesL, golesV;
};

struct Equipo {
    string nombre;
    int pj = 0, pg = 0, pe = 0, pp = 0;
    int gf = 0, gc = 0, dg = 0, pts = 0;
};

void actualizarEstadisticas(Equipo* e, int gF, int gC, int pV, int pE, int pD) {
    e->pj++;
    e->gf += gF;
    e->gc += gC;
    e->dg = e->gf - e->gc;

    if (gF > gC) {
        e->pg++;
        e->pts += pV;
    } else if (gF == gC) {
        e->pe++;
        e->pts += pE;
    } else {
        e->pp++;
        e->pts += pD;
    }
}

bool ordenar(const Equipo& a, const Equipo& b) {
    return a.pts > b.pts;
}

ConfigLiga cargarConfig() {
    ConfigLiga cfg;
    ifstream archivo("data/config.txt");
    if (!archivo.is_open()) {
        cout << "Error al abrir config.txt" << endl;
        exit(1);
    }

    string linea;
    while (getline(archivo, linea)) {
        if (linea.empty() || linea[0] == '#') continue;
        size_t pos = linea.find('=');
        string clave = linea.substr(0, pos);
        string valor = linea.substr(pos + 1);

        if (clave == "nombre_liga") cfg.nombre = valor;
        else if (clave == "pts_victoria") cfg.ptsV = stoi(valor);
        else if (clave == "pts_empate") cfg.ptsE = stoi(valor);
        else if (clave == "pts_derrota") cfg.ptsD = stoi(valor);
        else if (clave == "equipo") cfg.nombresEquipos.push_back(valor);
    }
    return cfg;
}

void verTabla(ConfigLiga cfg) {
    vector<Equipo> tabla;
    for (int i = 0; i < cfg.nombresEquipos.size(); i++) {
        Equipo eq;
        eq.nombre = cfg.nombresEquipos[i];
        tabla.push_back(eq);
    }

    ifstream archivo("data/partidos.txt");
    string linea;
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string f, l, v, gl, gv;
        getline(ss, f, ','); getline(ss, l, ','); getline(ss, v, ',');
        getline(ss, gl, ','); getline(ss, gv, ',');

        for (int i = 0; i < tabla.size(); i++) {
            if (tabla[i].nombre == l) 
                actualizarEstadisticas(&tabla[i], stoi(gl), stoi(gv), cfg.ptsV, cfg.ptsE, cfg.ptsD);
            if (tabla[i].nombre == v) 
                actualizarEstadisticas(&tabla[i], stoi(gv), stoi(gl), cfg.ptsV, cfg.ptsE, cfg.ptsD);
        }
    }
    archivo.close();

    sort(tabla.begin(), tabla.end(), ordenar);

    cout << "\n--- " << cfg.nombre << " ---\n";
    cout << "EQ\tPJ\tPG\tPE\tPP\tGF\tGC\tDG\tPTS" << endl;
    for (int i = 0; i < tabla.size(); i++) {
        cout << tabla[i].nombre << "\t" << tabla[i].pj << "\t" << tabla[i].pg << "\t" 
             << tabla[i].pe << "\t" << tabla[i].pp << "\t" << tabla[i].gf << "\t" 
             << tabla[i].gc << "\t" << tabla[i].dg << "\t" << tabla[i].pts << endl;
    }
}

void registrar(ConfigLiga cfg) {
    Partido p;
    cout << "Fecha: "; cin >> p.fecha;
    cout << "Local: "; cin >> p.local;
    cout << "Visitante: "; cin >> p.visitante;
    cout << "Goles local: "; cin >> p.golesL;
    cout << "Goles visitante: "; cin >> p.golesV;

    ofstream f1("data/partidos.txt", ios::app);
    f1 << p.fecha << "," << p.local << "," << p.visitante << "," << p.golesL << "," << p.golesV << endl;
    f1.close();

    ofstream f2("data/fechas.txt", ios::app);
    f2 << "JORNADA=" << p.fecha << endl;
    f2 << p.local << " " << p.golesL << "-" << p.golesV << " " << p.visitante << endl;
    f2 << "FIN_JORNADA" << endl;
    f2.close();
}

int main() {
    ConfigLiga cfg = cargarConfig();
    int opc = 0;
    while (opc != 4) {
        cout << "\n1. Tabla\n2. Registrar\n3. Historial\n4. Salir\nOpcion: ";
        cin >> opc;
        if (opc == 1) verTabla(cfg);
        if (opc == 2) registrar(cfg);
        if (opc == 3) {
            ifstream f("data/fechas.txt");
            string l;
            while (getline(f, l)) cout << l << endl;
            f.close();
        }
    }
    return 0;
}