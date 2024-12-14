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

void processData(const char* fileName, TH1F* dielectron_data_h)
{
    // Abrir o arquivo e obter o TTree
    TFile *file = new TFile(fileName);
    TTree *tree = (TTree*)file->Get("Electrons");

    // Definir as variáveis para as branches do TTree
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

    // Criar objetos para Lorentz Vectors
    TLorentzVector v1, v2, dielectron;

    // Loop sobre os eventos
    for (int iev = 0; iev < nevt; iev++) {
        tree->GetEntry(iev);
        int nElectrons = Electron_pt->size();
        if (nElectrons < 4) continue;

        // Loop sobre os pares de elétrons
        for (Int_t i = 0; i < nElectrons; i++) {
            if ((*Electron_pt)[i] < 30) continue;
            if (std::abs((*Electron_eta)[i]) > 2.4) continue;
            for (Int_t j = 0; j < i; j++) {
                if ((*Electron_pt)[j] < 30) continue;
                if (std::abs((*Electron_eta)[j]) > 2.4) continue;
                if ((*Electron_charge)[i] + (*Electron_charge)[j] != 0) continue;
                v1.SetPtEtaPhiM((*Electron_pt)[i], (*Electron_eta)[i], (*Electron_phi)[i], 0.000511);
                v2.SetPtEtaPhiM((*Electron_pt)[j], (*Electron_eta)[j], (*Electron_phi)[j], 0.000511);
                dielectron = v1 + v2;
                dielectron_data_h->Fill(dielectron.M());
            }
        }
    }
}

void plotComparison(TH1F* dielectron_data_h, TH1F* dielectron_mc_h)
{
    // Criar o canvas para os plots
    TCanvas *c1 = new TCanvas("c1", "Comparacao entre Dados e MC", 1000, 800);
    c1->SetTickx();
    c1->SetTicky();

    // Definir o estilo dos histogramas
    dielectron_data_h->SetLineColor(kBlack);
    dielectron_data_h->SetMarkerStyle(20);
    dielectron_data_h->SetMarkerSize(0.8);
    dielectron_data_h->SetStats(kFALSE);
    dielectron_mc_h->SetLineColor(kRed);
    dielectron_mc_h->SetMarkerStyle(21);
    dielectron_mc_h->SetMarkerSize(0.8);
    dielectron_mc_h->SetStats(kFALSE);

    // Configurar o eixo X
    dielectron_data_h->GetXaxis()->SetTitle("M_{e^{+}e^{-}} [GeV]");
    dielectron_data_h->GetYaxis()->SetTitle("Eventos / 0.25 GeV");

    // Desenhar os histogramas
    dielectron_mc_h->Draw("Hist");
    dielectron_data_h->Draw("E1 SAME");

    // Criar a legenda
    TLegend *legend = new TLegend(0.65, 0.65, 0.9, 0.85);
    legend->SetBorderSize(0);
    legend->SetFillStyle(0);
    legend->AddEntry(dielectron_data_h, "Dados", "lep");
    legend->AddEntry(dielectron_mc_h, "MC", "lep");
    legend->Draw();

    // Adicionar texto de explicação
    TLatex latex;
    latex.SetNDC();
    latex.SetTextSize(0.045);
    latex.SetTextAlign(13);
    latex.SetTextFont(42);
    latex.DrawLatex(0.10, 0.94, "CMS Open Data");
    latex.SetTextSize(0.035);
    latex.DrawLatex(0.15, 0.85, "pp 2016, #sqrt{s_{NN}} = 13 TeV");
    latex.DrawLatex(0.15, 0.80, "p^{e}_{T} > 30 GeV, |#eta^{e}| < 2.4");
    latex.Draw();

    // Salvar o gráfico
    c1->SaveAs("Data_vs_MC_Comparison.png");
}

void analise3()
{
    // Criar os histogramas para Dados e MC
    TH1F *dielectron_data_h = new TH1F("dielelectron_data_h", "; ; Eventos / 0.25 GeV", 100, 80, 100);
    TH1F *dielectron_mc_h = new TH1F("dielelectron_mc_h", "; ; Eventos / 0.25 GeV", 100, 80, 100);

    // Processar os dados de MC e Data
    processData("MC.root", dielectron_mc_h);
    processData("Data.root", dielectron_data_h);

    // Plotar a comparação entre Dados e MC
    plotComparison(dielectron_data_h, dielectron_mc_h);
}
