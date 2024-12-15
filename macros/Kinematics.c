#include <TMath.h>
#include <TLorentzVector.h>
#include <TTree.h>
#include <TFile.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLatex.h>
#include <iostream>
#include <vector>

void Kinematics()
{
    // Abrir o arquivo ROOT
    TFile *file = new TFile("/home/tandrade/Projeto final/Data/MC.root");
    TTree *tree = (TTree*)file->Get("Electrons");

    // Criar histogramas para variáveis cinemáticas dos dieletrons (antes dos cortes)
    TH1F *pt_h_before = new TH1F("pt_h_before", "Dielectron pT (Before Cuts); pT [GeV]; Events", 100, 0, 200);
    TH1F *eta_h_before = new TH1F("eta_h_before", "Dielectron Eta (Before Cuts); Eta; Events", 100, -2.5, 2.5);
    TH1F *phi_h_before = new TH1F("phi_h_before", "Dielectron Phi (Before Cuts); Phi; Events", 100, -3.14, 3.14);
    TH1F *mass_h_before = new TH1F("mass_h_before", "Dielectron Mass (Before Cuts); Mass [GeV]; Events / 0.25 GeV", 100, 80, 100);

    // Criar histogramas para variáveis cinemáticas dos dieletrons (após os cortes)
    TH1F *pt_h_after = new TH1F("pt_h_after", "Dielectron pT (After Cuts); pT [GeV]; Events", 100, 0, 200);
    TH1F *eta_h_after = new TH1F("eta_h_after", "Dielectron Eta (After Cuts); Eta; Events", 100, -2.5, 2.5);
    TH1F *phi_h_after = new TH1F("phi_h_after", "Dielectron Phi (After Cuts); Phi; Events", 100, -3.14, 3.14);
    TH1F *mass_h_after = new TH1F("mass_h_after", "Dielectron Mass (After Cuts); Mass [GeV]; Events / 0.25 GeV", 100, 80, 100);

    // Variáveis para leitura da árvore
    std::vector<float> *Electron_pt = nullptr;
    std::vector<int> *Electron_charge = nullptr;
    std::vector<float> *Electron_eta = nullptr;
    std::vector<float> *Electron_phi = nullptr;

    tree->SetBranchAddress("Electron_pt", &Electron_pt);
    tree->SetBranchAddress("Electron_charge", &Electron_charge);
    tree->SetBranchAddress("Electron_eta", &Electron_eta);
    tree->SetBranchAddress("Electron_phi", &Electron_phi);

    int nevt = tree->GetEntries();
    std::cout << "Eventos para analisar: " << nevt << std::endl;

    TLorentzVector v1, v2, dielectron;

    // Loop sobre os eventos antes dos cortes
    for (int iev = 0; iev < nevt; iev++) {
        tree->GetEntry(iev);
        int nElectrons = Electron_pt->size();
        if (nElectrons < 4) continue; // Precisa de ao menos dois elétrons
        for (Int_t i = 0; i < nElectrons; i++) {
            for (Int_t j = 0; j < i; j++) {
                v1.SetPtEtaPhiM((*Electron_pt)[i], (*Electron_eta)[i], (*Electron_phi)[i], 0.000511);
                v2.SetPtEtaPhiM((*Electron_pt)[j], (*Electron_eta)[j], (*Electron_phi)[j], 0.000511);
                dielectron = v1 + v2;
                pt_h_before->Fill(dielectron.Pt());
                eta_h_before->Fill(dielectron.Eta());
                phi_h_before->Fill(dielectron.Phi());
                mass_h_before->Fill(dielectron.M());
            }
        }
    }

    // Plotar resultados antes dos cortes
    TCanvas *c1 = new TCanvas("c1", "Dielectron Kinematics (Before Cuts)", 1200, 800);
    c1->Divide(2, 2);

    c1->cd(1); pt_h_before->Draw();
    c1->cd(2); eta_h_before->Draw();
    c1->cd(3); phi_h_before->Draw();
    c1->cd(4); mass_h_before->Draw();
    c1->SaveAs("dielectron_kinematics_before_cuts_MC.png");

    // Loop sobre os eventos após aplicar os cortes
    for (int iev = 0; iev < nevt; iev++) {
        tree->GetEntry(iev);
        int nElectrons = Electron_pt->size();
        if (nElectrons < 4) continue;
        for (Int_t i = 0; i < nElectrons; i++) {
            for (Int_t j = 0; j < i; j++) {
                if ((*Electron_charge)[i] + (*Electron_charge)[j] != 0) continue;
                if ((*Electron_pt)[i] < 30 || (*Electron_pt)[j] < 30) continue;
                if (fabs((*Electron_eta)[i]) > 2.4 || fabs((*Electron_eta)[j]) > 2.4) continue;
                v1.SetPtEtaPhiM((*Electron_pt)[i], (*Electron_eta)[i], (*Electron_phi)[i], 0.000511);
                v2.SetPtEtaPhiM((*Electron_pt)[j], (*Electron_eta)[j], (*Electron_phi)[j], 0.000511);
                dielectron = v1 + v2;
                
                pt_h_after->Fill(dielectron.Pt());
                eta_h_after->Fill(dielectron.Eta());
                phi_h_after->Fill(dielectron.Phi());
                mass_h_after->Fill(dielectron.M());
            }
        }
    }

    // Plotar resultados após os cortes
    TCanvas *c2 = new TCanvas("c2", "Dielectron Kinematics (After Cuts)", 1200, 800);
    c2->Divide(2, 2);

    c2->cd(1); pt_h_after->Draw();
    c2->cd(2); eta_h_after->Draw();
    c2->cd(3); phi_h_after->Draw();
    c2->cd(4); mass_h_after->Draw();

    c2->SaveAs("dielectron_kinematics_after_cuts_MC.png");

    // Limpar memória
    delete file;
    delete c1;
    delete c2;
}
