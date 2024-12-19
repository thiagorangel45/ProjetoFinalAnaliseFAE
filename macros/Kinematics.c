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

void Kinematics() {
    // Abrir o arquivo ROOT
    TFile *file = new TFile("/Users/thiagorangel/UERJ/Introducao_Analise_de_Dados_FAE/Projeto_Final_FAE/Data/MC.root");
    TTree *tree = (TTree*)file->Get("Electrons");

    // Criar histogramas para variáveis cinemáticas dos dieletrons (antes e após os cortes)
    TH1F *pt_h_before_cut = new TH1F("pt_h_before_cut", "; p^{ee}_{T} [GeV]; Events", 100, 0, 200);
    TH1F *eta_h_before_cut = new TH1F("eta_h_before_cut", "; #eta^{ee}; Events", 50, -5, 5);
    TH1F *phi_h_before_cut = new TH1F("phi_h_before_cut", "; #phi^{ee}; Events", 50, -5, 5);
    TH1F *mass_h_before_cut = new TH1F("mass_h_before_cut", "; m_{e^{+}e^{-}} [GeV]; N_{Events} / (1 GeV)", 150, 0, 150);

    TH1F *pt_h_after_eta_cut = new TH1F("pt_h_after_eta_cut", "; p^{ee}_{T} [GeV]; Events", 100, 0, 200);
    TH1F *eta_h_after_eta_cut = new TH1F("eta_h_after_eta_cut", "; #eta^{ee}; Events", 50, -5, 5);
    TH1F *phi_h_after_eta_cut = new TH1F("phi_h_after_eta_cut", "; #phi^{ee}; Events", 50, -4, 4);
    TH1F *mass_h_after_eta_cut = new TH1F("mass_h_after_eta_cut", "; m_{e^{+}e^{-}} [GeV]; N_{Events} / (1 Gev)", 150, 0, 150);

    TH1F *pt_h_after_pt_cut = new TH1F("pt_h_after_pt_cut", "; p^{ee}_{T} [GeV]; Events", 100, 0, 200);
    TH1F *eta_h_after_pt_cut = new TH1F("eta_h_after_pt_cut", "; #eta^{ee}; Events", 50, -5, 5);
    TH1F *phi_h_after_pt_cut = new TH1F("phi_h_after_pt_cut", "; #phi^{ee}; Events", 50, -4, 4);
    TH1F *mass_h_after_pt_cut = new TH1F("mass_h_after_pt_cut", ";m_{e^{+}e^{-}} [GeV]; N_{Events} / (1 GeV)", 150, 0, 150);

    TH1F *pt_h_after_all_cut = new TH1F("pt_h_after_all_cut", "; p^{ee}_{T} [GeV]; Events", 100, 0, 200);
    TH1F *eta_h_after_all_cut = new TH1F("eta_h_after_all_cut", "; #eta^{ee}; Events", 50, -5, 5);
    TH1F *phi_h_after_all_cut = new TH1F("phi_h_after_all_cut", "; #phi^{ee}; Events", 50, -4, 4);
    TH1F *mass_h_after_all_cut = new TH1F("mass_h_after_all_cut", "; m_{e^{+}e^{-}} [GeV]; N_{Events} / (1 GeV)", 150, 0, 150);

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
        if (nElectrons < 4) continue;
        for (Int_t i = 0; i < nElectrons; i++) {
            for (Int_t j = 0; j < i; j++) {
                if ((*Electron_charge)[i] + (*Electron_charge)[j] != 0) continue;
                v1.SetPtEtaPhiM((*Electron_pt)[i], (*Electron_eta)[i], (*Electron_phi)[i], 0.000511);
                v2.SetPtEtaPhiM((*Electron_pt)[j], (*Electron_eta)[j], (*Electron_phi)[j], 0.000511);
                dielectron = v1 + v2;
                pt_h_before_cut->Fill(dielectron.Pt());
                eta_h_before_cut->Fill(dielectron.Eta());
                phi_h_before_cut->Fill(dielectron.Phi());
                mass_h_before_cut->Fill(dielectron.M());
            }
        }
    }

    // Loop sobre os eventos após o corte de eta
    for (int iev = 0; iev < nevt; iev++) {
        tree->GetEntry(iev);
        int nElectrons = Electron_pt->size();
        if (nElectrons < 4) continue;
        for (Int_t i = 0; i < nElectrons; i++) {
            if (TMath::Abs((*Electron_eta)[i]) > 2.4) continue;
            for (Int_t j = 0; j < i; j++) {
                if (TMath::Abs((*Electron_eta)[j]) > 2.4) continue;
                if ((*Electron_charge)[i] + (*Electron_charge)[j] != 0) continue;
                v1.SetPtEtaPhiM((*Electron_pt)[i], (*Electron_eta)[i], (*Electron_phi)[i], 0.000511);
                v2.SetPtEtaPhiM((*Electron_pt)[j], (*Electron_eta)[j], (*Electron_phi)[j], 0.000511);
                dielectron = v1 + v2;
                pt_h_after_eta_cut->Fill(dielectron.Pt());
                eta_h_after_eta_cut->Fill(dielectron.Eta());
                phi_h_after_eta_cut->Fill(dielectron.Phi());
                mass_h_after_eta_cut->Fill(dielectron.M());
            }
        }
    }

    // Loop sobre os eventos após o corte de pT
    for (int iev = 0; iev < nevt; iev++) {
        tree->GetEntry(iev);
        int nElectrons = Electron_pt->size();
        if (nElectrons < 4) continue;
        for (Int_t i = 0; i < nElectrons; i++) {
            if ((*Electron_pt)[i] < 10) continue;
            for (Int_t j = 0; j < i; j++) {
                if ((*Electron_pt)[j] < 10) continue;
                if ((*Electron_charge)[i] + (*Electron_charge)[j] != 0) continue;
                v1.SetPtEtaPhiM((*Electron_pt)[i], (*Electron_eta)[i], (*Electron_phi)[i], 0.000511);
                v2.SetPtEtaPhiM((*Electron_pt)[j], (*Electron_eta)[j], (*Electron_phi)[j], 0.000511);
                dielectron = v1 + v2;
                pt_h_after_pt_cut->Fill(dielectron.Pt());
                eta_h_after_pt_cut->Fill(dielectron.Eta());
                phi_h_after_pt_cut->Fill(dielectron.Phi());
                mass_h_after_pt_cut->Fill(dielectron.M());
            }
        }
    }

    // Loop sobre os eventos após o corte de pT e eta
    for (int iev = 0; iev < nevt; iev++) {
        tree->GetEntry(iev);
        int nElectrons = Electron_pt->size();
        if (nElectrons < 4) continue;
        for (Int_t i = 0; i < nElectrons; i++) {
            if (TMath::Abs((*Electron_eta)[i]) > 2.4) continue;
            if ((*Electron_pt)[i] < 30) continue;
            for (Int_t j = 0; j < i; j++) {
                if (TMath::Abs((*Electron_eta)[j]) > 2.4) continue;
                if ((*Electron_pt)[j] < 30) continue;
                if ((*Electron_charge)[i] + (*Electron_charge)[j] != 0) continue;
                v1.SetPtEtaPhiM((*Electron_pt)[i], (*Electron_eta)[i], (*Electron_phi)[i], 0.000511);
                v2.SetPtEtaPhiM((*Electron_pt)[j], (*Electron_eta)[j], (*Electron_phi)[j], 0.000511);
                dielectron = v1 + v2;
                pt_h_after_all_cut->Fill(dielectron.Pt());
                eta_h_after_all_cut->Fill(dielectron.Eta());
                phi_h_after_all_cut->Fill(dielectron.Phi());
                mass_h_after_all_cut->Fill(dielectron.M());
            }
        }
    }

    // Plotando os histogramas de pT
TCanvas *c1 = new TCanvas("c1", "Histograma pT", 800, 600);
pt_h_before_cut->SetLineColor(kBlack);
pt_h_before_cut->SetStats(kFALSE);
pt_h_after_eta_cut->SetLineColor(kRed);
pt_h_after_pt_cut->SetLineColor(kBlue);
pt_h_after_all_cut->SetLineColor(kGreen);
pt_h_before_cut->Draw();
pt_h_after_eta_cut->Draw("same");
pt_h_after_pt_cut->Draw("same");
pt_h_after_all_cut->Draw("same");

// Adicionando a legenda
TLegend *legend1 = new TLegend(0.7, 0.7, 0.9, 0.9);
legend1->AddEntry(pt_h_before_cut, "Antes dos Cortes", "l");
legend1->AddEntry(pt_h_after_eta_cut, "Apos Cortes de |#eta^{e}| < 2.4", "l");
legend1->AddEntry(pt_h_after_pt_cut, "Apos Cortes de p^{e}_{T} > 30 GeV", "l");
legend1->AddEntry(pt_h_after_all_cut, "Apos Todos os Cortes Juntos", "l");
legend1->Draw();
c1->SaveAs("pt_histograms.pdf");
c1->SaveAs("pt_histograms.png");

// Plotando os histogramas de Eta
TCanvas *c2 = new TCanvas("c2", "Histograma Eta", 800, 600);
eta_h_before_cut->SetLineColor(kBlack);
eta_h_before_cut->SetStats(kFALSE);
eta_h_after_eta_cut->SetLineColor(kRed);
eta_h_after_pt_cut->SetLineColor(kBlue);
eta_h_after_all_cut->SetLineColor(kGreen);
eta_h_before_cut->Draw();
eta_h_after_eta_cut->Draw("same");
eta_h_after_pt_cut->Draw("same");
eta_h_after_all_cut->Draw("same");

TLegend *legend2 = new TLegend(0.7, 0.7, 0.9, 0.9);
legend2->AddEntry(eta_h_before_cut, "Antes dos Cortes", "l");
legend2->AddEntry(eta_h_after_eta_cut, "Apos Cortes de |#eta^{e}| < 2.4", "l");
legend2->AddEntry(eta_h_after_pt_cut, "Apos Cortes de p^{e}_{T} > 30 GeV", "l");
legend2->AddEntry(eta_h_after_all_cut, "Apos Todos os Cortes Juntos", "l");
legend2->Draw();
c2->SaveAs("eta_histograms.pdf");
c2->SaveAs("eta_histograms.png");


// Plotando os histogramas de Phi
TCanvas *c3 = new TCanvas("c3", "Histograma Phi", 800, 600);
phi_h_before_cut->SetLineColor(kBlack);
phi_h_before_cut->SetStats(kFALSE);
phi_h_after_eta_cut->SetLineColor(kRed);
phi_h_after_pt_cut->SetLineColor(kBlue);
phi_h_after_all_cut->SetLineColor(kGreen);
phi_h_before_cut->Draw();
phi_h_after_eta_cut->Draw("same");
phi_h_after_pt_cut->Draw("same");
phi_h_after_all_cut->Draw("same");

TLegend *legend3 = new TLegend(0.7, 0.7, 0.9, 0.9);
legend3->AddEntry(phi_h_before_cut, "Antes dos Cortes", "l");
legend3->AddEntry(phi_h_after_eta_cut, "Apos Cortes de #|Eta^{e}| < 2.4", "l");
legend3->AddEntry(phi_h_after_pt_cut, "Apos Cortes de p^{e}_{T} > 30 GeV", "l");
legend3->AddEntry(phi_h_after_all_cut, "Apos Todos os Cortes Juntos", "l");
legend3->Draw();
c3->SaveAs("phi_histograms.pdf");
c3->SaveAs("phi_histograms.png");


// Plotando os histogramas de Massa
TCanvas *c4 = new TCanvas("c4", "Histograma Massa", 800, 600);
mass_h_before_cut->SetLineColor(kBlack);
mass_h_before_cut->SetStats(kFALSE);
mass_h_after_eta_cut->SetLineColor(kRed);
mass_h_after_pt_cut->SetLineColor(kBlue);
mass_h_after_all_cut->SetLineColor(kGreen);
mass_h_before_cut->Draw();
mass_h_after_eta_cut->Draw("same");
mass_h_after_pt_cut->Draw("same");
mass_h_after_all_cut->Draw("same");

TLegend *legend4 = new TLegend(0.7, 0.7, 0.9, 0.9);
legend4->AddEntry(mass_h_before_cut, "Antes dos Cortes", "l");
legend4->AddEntry(mass_h_after_eta_cut, "Apos Cortes de |#eta^{e}| < 2.4", "l");
legend4->AddEntry(mass_h_after_pt_cut, "Apos Cortes de p^{e}_{T} > 30 GeV", "l");
legend4->AddEntry(mass_h_after_all_cut, "Apos Todos os Cortes Juntos", "l");
legend4->Draw();
c4->SaveAs("mass_histograms.pdf");
c4->SaveAs("mass_histograms.png");
}
