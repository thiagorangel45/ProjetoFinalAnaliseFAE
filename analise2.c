#include <TMath.h>
#include <TLorentzVector.h>
#include <TTree.h>
#include <TFile.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <iostream>
#include <vector>

void analyze(TTree* tree, TH1F* histogram, const std::string& treeName) {
    std::vector<float> *Electron_pt = nullptr;
    std::vector<int> *Electron_charge = nullptr;
    std::vector<float> *Electron_eta = nullptr;
    std::vector<float> *Electron_phi = nullptr;

    tree->SetBranchAddress("Electron_pt", &Electron_pt);
    tree->SetBranchAddress("Electron_charge", &Electron_charge);
    tree->SetBranchAddress("Electron_eta", &Electron_eta);
    tree->SetBranchAddress("Electron_phi", &Electron_phi);

    int nevt = tree->GetEntries();
    std::cout << "Eventos para analisar em " << treeName << ": " << nevt << std::endl;

    TLorentzVector v1, v2, dielectron;

    for (int iev = 0; iev < nevt; iev++) {
        if (iev % 10000 == 0) {
            std::cout << ">>>>> EVENTO " << iev << " (" << treeName << ")" << std::endl;
        }

        tree->GetEntry(iev);

        int nElectrons = Electron_pt->size();
        if (nElectrons < 4) continue;

        for (Int_t i = 0; i < nElectrons; i++) {
            //if ((*Electron_pt)[i] < 30) continue;
            //if (std::abs((*Electron_eta)[i]) > 2.4) continue;

            for (Int_t j = 0; j < i; j++) {
                //if ((*Electron_pt)[j] < 30) continue;
                //if (std::abs((*Electron_eta)[j]) > 2.4) continue;

                if ((*Electron_charge)[i] + (*Electron_charge)[j] != 0) continue;

                v1.SetPtEtaPhiM((*Electron_pt)[i], (*Electron_eta)[i], (*Electron_phi)[i], 0.000511);
                v2.SetPtEtaPhiM((*Electron_pt)[j], (*Electron_eta)[j], (*Electron_phi)[j], 0.000511);
                dielectron = v1 + v2;    
                histogram->Fill(dielectron.M());
            }
        }
    }
}

void analise2() {
    TFile *Datafile = new TFile("Data.root");
    if (!Datafile || Datafile->IsZombie()) {
        std::cout << "Erro ao abrir o arquivo 'Data.root'!" << std::endl;
        return;
    }

    TFile *MCfile = new TFile("MC.root");
    if (!MCfile || MCfile->IsZombie()) {
        std::cout << "Erro ao abrir o arquivo 'MC.root'!" << std::endl;
        Datafile->Close();
        delete Datafile;
        return;
    }

    TTree *DataElectronTree = (TTree*)Datafile->Get("SelectedElectrons");
    if (!DataElectronTree) {
        std::cout << "Erro: TTree 'SelectedElectrons' não encontrado no arquivo 'Data.root'!" << std::endl;
        Datafile->Close();
        delete Datafile;
        return;
    }

    TTree *MCElectronTree = (TTree*)MCfile->Get("Electrons");
    if (!MCElectronTree) {
        std::cout << "Erro: TTree 'Electrons' não encontrado no arquivo 'MC.root'!" << std::endl;
        Datafile->Close();
        MCfile->Close();
        delete Datafile;
        delete MCfile;
        return;
    }

    TCanvas *c1 = new TCanvas("c1", "Data", 800, 600);
    TH1F *dielectron_data_h = new TH1F("dielelectron_data_h",
                                       "Distribuicao de Massa do Par de Particulas (Data);Massa Invariante [GeV/c^{2}];Eventos",
                                       400, 0, 400);

    TCanvas *c2 = new TCanvas("c2", "MC", 800, 600);
    TH1F *dielectron_mc_h = new TH1F("dielelectron_mc_h",
                                     "Distribuicao de Massa do Par de Particulas (MC);Massa Invariante [GeV/c^{2}];Eventos",
                                     400, 0, 400);

    analyze(DataElectronTree, dielectron_data_h, "Data");
    analyze(MCElectronTree, dielectron_mc_h, "MC");

    c1->cd();
    dielectron_data_h->Draw();
    c1->SaveAs("DATA.png");
    c2->cd();
    dielectron_mc_h->Draw();
    c2->SaveAs("MC.png");

    Datafile->Close();
    MCfile->Close();
    delete Datafile;
    delete MCfile;
}
