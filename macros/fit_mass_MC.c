#include <TMath.h>
#include <TLorentzVector.h>
#include <TTree.h>
#include <TFile.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLatex.h>
#include <RooRealVar.h>
#include <RooDataHist.h>
#include <RooPlot.h>
#include <RooAddPdf.h>
#include <RooGaussian.h>
#include <RooCBShape.h>
#include <RooFitResult.h>
#include <iostream>
#include <vector>

void fit_mass_MC()
{
    TFile *file = new TFile("/Users/thiagorangel/UERJ/Introducao_Analise_de_Dados_FAE/Projeto_Final_FAE/Data/MC.root");
    TTree *tree = (TTree*)file->Get("Electrons");
    TH1F *dielectron_data_h = new TH1F("dielelectron_data_h",
                                       "; ; Events / 0.25 GeV",
                                       100, 80, 100);

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
        if (iev % 10000 == 0) {
            std::cout << ">>>>> EVENTO " << iev << std::endl;
        }

        tree->GetEntry(iev);
        int nElectrons = Electron_pt->size();
        if (nElectrons < 4) continue;

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

    RooRealVar mass("mass", "m_{e^{+}e^{-}} [GeV]", 80, 100);
    RooDataHist data("data", "Dataset from dielectron histogram", mass, RooFit::Import(*dielectron_data_h));

    RooRealVar cb_mean("cb_mean", "CB Mean", 91, 80, 100);
    RooRealVar cb_sigma("cb_sigma", "CB Sigma", 2, 0.1, 10);
    RooRealVar cb_alpha("cb_alpha", "CB Alpha", 1.5, 0.1, 5);
    RooRealVar cb_n("cb_n", "CB n", 3, 0.1, 10);
    RooCBShape cb_signal("cb_signal", "Crystal Ball Signal", mass, cb_mean, cb_sigma, cb_alpha, cb_n);

    RooRealVar gauss_mean("gauss_mean", "Gauss Mean", 85, 80, 90);
    RooRealVar gauss_sigma("gauss_sigma", "Gauss Sigma", 5, 0.1, 10);
    RooGaussian gauss_bkg("gauss_bkg", "Gaussian Background", mass, gauss_mean, gauss_sigma);

    RooRealVar nsig("nsig", "Signal Yield", 1000, 0, 1e6);
    RooRealVar nbkg("nbkg", "Background Yield", 100, 0, 1e6);

    RooAddPdf model("model", "Signal + Background", RooArgList(cb_signal, gauss_bkg), RooArgList(nsig, nbkg));
    RooFitResult *fitResult = model.fitTo(data, RooFit::Save());

    TCanvas *c1 = new TCanvas("c1", "Ajuste com RooFit", 1000, 800);
    c1->SetTickx();
    c1->SetTicky();
    RooPlot *frame = mass.frame();
    data.plotOn(frame);
    model.plotOn(frame);
    model.plotOn(frame, RooFit::Components("cb_signal"), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
    model.plotOn(frame, RooFit::Components("gauss_bkg"), RooFit::LineColor(kGreen), RooFit::LineStyle(kDashed));

    frame->SetTitle("");
    frame->Draw();

    TLegend *legend = new TLegend(0.65, 0.65, 0.9, 0.85);
    legend->SetBorderSize(0);
    legend->SetFillStyle(0);
    legend->AddEntry(frame->findObject("data"), "Data", "lep");
    legend->AddEntry(frame->findObject("model_Norm[mass]"), "Total Fit", "l");

    TLegendEntry *entry1 = legend->AddEntry(frame->findObject("cb_signal"), "CB Signal", "l");
    entry1->SetLineColor(kRed);  // Cor do CB Signal
    entry1->SetLineStyle(kDashed);
    TLegendEntry *entry2 = legend->AddEntry(frame->findObject("gauss_bkg"), "Gaussian Background", "l");
    entry2->SetLineColor(kGreen);  // Cor do Gaussian Background
    entry2->SetLineStyle(kDashed);
    legend->Draw();

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

    c1->SaveAs("FitResult_MC.png");
    fitResult->Print();
}
