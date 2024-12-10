#include <TMath.h>
#include <TLorentzVector.h>
#include <TTree.h>
#include <TFile.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>

using std::cout;
using std::endl;
using std::vector;
using std::pair;

void analise() 
{  
    // Constante: massa do elétron (em GeV/c²)
    const float electronMass = 0.000511;
    const float Z_mass = 91.2; // Massa do bóson Z em GeV

    // Abrir arquivo ROOT
    TFile *Datafile = new TFile("Data.root");
    if (!Datafile || Datafile->IsZombie()) {
        cout << "Erro ao abrir o arquivo 'Data.root'!" << endl;
        return;
    }

    TTree *ElectronTree = (TTree*)Datafile->Get("SelectedElectrons");
    if (!ElectronTree) {
        cout << "Erro: TTree 'SelectedElectrons' não encontrado no arquivo 'Data.root'!" << endl;
        Datafile->Close();
        delete Datafile;
        return;
    }

    // Variáveis para as partículas carregadas (elétrons/positrons)
    vector<float> *Particle_pt = nullptr;
    vector<int> *Particle_charge = nullptr;
    vector<float> *Particle_eta = nullptr;
    vector<float> *Particle_phi = nullptr;

    // Configurar branches
    ElectronTree->SetBranchAddress("Electron_pt", &Particle_pt);
    ElectronTree->SetBranchAddress("Electron_charge", &Particle_charge);
    ElectronTree->SetBranchAddress("Electron_eta", &Particle_eta);
    ElectronTree->SetBranchAddress("Electron_phi", &Particle_phi);

    // Criar histograma para a massa do par de partículas
    TH1F *dielectron_h = new TH1F("dielectron_h", "Distribuicao de Massa do Par de Particulas;Massa Invariante [GeV/c^{2}];Eventos", 20, 80, 100);

    // Loop sobre os eventos
    int nevt = ElectronTree->GetEntries();
    cout << "Eventos para analisar: " << nevt << endl;

    TLorentzVector v1, v2, pairVar; // Vetores de Lorentz para as partículas e o par

    for (int iev = 0; iev < nevt; iev++) {
        if (iev % 10000 == 0) {
            cout << ">>>>> EVENTO " << iev << endl; 
        }

        ElectronTree->GetEntry(iev);

        int N_particles = Particle_pt->size(); // Número de partículas no evento
        if (N_particles < 4) continue; // Exige pelo menos 4 partículas no evento (para formar dois pares)

        // Vetores para armazenar partículas carregadas (eletrons e positrons)
        vector<pair<int, TLorentzVector>> particles; // Índice e vetor de Lorentz

        for (int i = 0; i < N_particles; i++) {
            if (Particle_pt->at(i) < 10.0) continue; // Cortes básicos
            if (abs(Particle_eta->at(i)) > 2.4) continue;

            TLorentzVector particle;
            particle.SetPtEtaPhiM(Particle_pt->at(i), Particle_eta->at(i), Particle_phi->at(i), electronMass);
            particles.push_back({i, particle});
        }

        if (particles.size() < 4) continue; // Checagem extra após os cortes

        // Ordenar as partículas por pT (decrescente)
        std::sort(particles.begin(), particles.end(), [](const pair<int, TLorentzVector>& a, const pair<int, TLorentzVector>& b) {
            return a.second.Pt() > b.second.Pt();
        });

        // Criar um vetor para armazenar as combinações de pares
        vector<pair<float, pair<int, int>>> possiblePairs;

        // Gerar as combinações de pares
        for (size_t i = 0; i < particles.size(); i++) {
            for (size_t j = i + 1; j < particles.size(); j++) {
                if (Particle_charge->at(particles[i].first) * Particle_charge->at(particles[j].first) >= 0) continue; // Carga oposta

                pairVar = particles[i].second + particles[j].second;
                float mass = pairVar.M();
                float massDiff = std::abs(mass - Z_mass);

                possiblePairs.push_back({massDiff, {particles[i].first, particles[j].first}});
            }
        }

        // Ordenar os pares pela proximidade com a massa do Z
        std::sort(possiblePairs.begin(), possiblePairs.end(), [](const pair<float, pair<int, int>>& a, const pair<float, pair<int, int>>& b) {
            return a.first < b.first; // Ordena pela diferença de massa (mais próximo da massa do Z)
        });

        // Vetor para marcar os elétrons já utilizados
        vector<bool> usedParticles(particles.size(), false);

        // Adicionar pares ao histograma, evitando repetição de partículas
        for (size_t i = 0; i < possiblePairs.size(); i++) {
            int idx1 = possiblePairs[i].second.first;
            int idx2 = possiblePairs[i].second.second;

            // Verificar se as partículas já foram usadas em um par anterior
            if (usedParticles[idx1] || usedParticles[idx2]) continue;

            TLorentzVector finalPair = particles[idx1].second + particles[idx2].second;
            dielectron_h->Fill(finalPair.M());  // Preencher o histograma com a massa invariante

            // Marcar as partículas como usadas
            usedParticles[idx1] = true;
            usedParticles[idx2] = true;
        }
    }

    // Desenhar o histograma
    TCanvas *c1 = new TCanvas("c1", "Massa do Par de Particulas", 800, 600);
    dielectron_h->Draw("HIST");
    c1->SaveAs("pair_mass_DATA.png");

    // Limpar memória
    delete c1;
    delete dielectron_h;
    Datafile->Close();
    delete Datafile;

    cout << "Análise concluída e arquivo salvo!" << endl;
}
