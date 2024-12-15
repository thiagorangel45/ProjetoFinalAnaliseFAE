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

void processData(const char* fileName, TH1F* dielectron_data_h, bool isMC)
{
    TFile *file = new TFile(fileName);
    TTree *tree = (TTree*)file->Get("Electrons");

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

    for (int iev = 0; iev < nevt; iev++) {
        tree->GetEntry(iev);
        int nElectrons = Electron_pt->size();
        if (nElectrons < 4) continue;

        float ptCut = isMC ? 30.0 : 70.0;

        for (Int_t i = 0; i < nElectrons; i++) {
            if ((*Electron_pt)[i] < ptCut) continue;
            if (std::abs((*Electron_eta)[i]) > 2.4) continue;
            for (Int_t j = 0; j < i; j++) {
                if ((*Electron_pt)[j] < ptCut) continue;
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

void normalizeHistogram(TH1F* hist)
{
    Double_t integral = hist->Integral();
    if (integral != 0) {
        hist->Scale(1.0 / integral);
    } else {
        std::cout << "Aviso: O histograma tem integral zero e não pode ser normalizado!" << std::endl;
    }
}

void plotComparison(TH1F* dielectron_data_h, TH1F* dielectron_mc_h)
{
    TCanvas *c1 = new TCanvas("c1", "Comparacao entre Dados e MC", 1000, 800);
    c1->SetTickx();
    c1->SetTicky();

    // Configuração do histograma dos dados
    dielectron_data_h->SetLineColor(kBlack);  // Linha preta para o histograma de dados
    dielectron_data_h->SetMarkerStyle(20);    // Estilo do marcador para os dados (pontos)
    dielectron_data_h->SetMarkerSize(0.8);    // Ajuste do tamanho dos pontos (barra de erro)
    dielectron_data_h->SetStats(kFALSE);

    // Configuração do histograma do MC
    dielectron_mc_h->SetLineColor(kBlue);  // Histograma MC em azul
    dielectron_mc_h->SetFillColorAlpha(kBlue, 0.5); // Azul com opacidade de 0.5
    dielectron_mc_h->SetStats(kFALSE);

    // Definindo o intervalo do eixo Y
    dielectron_data_h->GetYaxis()->SetRangeUser(0, 0.15);
    dielectron_mc_h->GetYaxis()->SetRangeUser(0, 0.15);

    dielectron_data_h->GetXaxis()->SetTitle("M_{e^{+}e^{-}} [GeV]");
    dielectron_data_h->GetYaxis()->SetTitle("Eventos Normalizados / 0.25 GeV");

    // Desenho da banda de erro primeiro (MC)
    dielectron_mc_h->Draw("E2"); // Desenhar a banda de erro do MC (com incerteza)
    dielectron_mc_h->Draw("HIST SAME"); // Desenhar o histograma de MC por cima da banda de erro

    // Desenho do histograma de dados com barras de erro
    dielectron_data_h->Draw("E1 SAME"); // Dados com barras de erro sobre a banda de erro

    // Legenda
    TLegend *legend = new TLegend(0.65, 0.65, 0.9, 0.85);
    legend->SetBorderSize(0);
    legend->SetFillStyle(0);
    legend->AddEntry(dielectron_data_h, "Dados", "lep");
    legend->AddEntry(dielectron_mc_h, "MC", "f");
    legend->AddEntry((TObject*)nullptr, "Incerteza (#pm)", "f");
    legend->Draw();

    // Adicionar textos descritivos
    TLatex latex;
    latex.SetNDC();
    latex.SetTextSize(0.045);
    latex.SetTextAlign(13);
    latex.SetTextFont(42);
    latex.DrawLatex(0.10, 0.94, "CMS Open Data");
    latex.SetTextSize(0.035);
    latex.DrawLatex(0.15, 0.85, "pp 2016, #sqrt{s_{NN}} = 13 TeV");
    latex.DrawLatex(0.15, 0.80, "p^{e}_{T} > 30 GeV |#eta^{e}| < 2.4");
    latex.Draw();

    c1->SaveAs("Data_vs_MC_Comparison_Updated.png");
}

void Data_vs_MC_mass_2()
{
    TH1F *dielectron_data_h = new TH1F("dielelectron_data_h", "; ; Eventos Normalizados / 0.25 GeV", 20, 80, 100);
    dielectron_data_h->Sumw2();
    TH1F *dielectron_mc_h = new TH1F("dielelectron_mc_h", "; ; Eventos Normalizados / 0.25 GeV", 20, 80, 100);
    dielectron_mc_h->Sumw2();

    processData("/Users/thiagorangel/UERJ/Introducao_Analise_de_Dados_FAE/Projeto_Final_FAE/Data/MC.root", dielectron_mc_h, true);
    processData("/Users/thiagorangel/UERJ/Introducao_Analise_de_Dados_FAE/Projeto_Final_FAE/Data/Data.root", dielectron_data_h, false);

    normalizeHistogram(dielectron_data_h);
    normalizeHistogram(dielectron_mc_h);

    plotComparison(dielectron_data_h, dielectron_mc_h);
}
