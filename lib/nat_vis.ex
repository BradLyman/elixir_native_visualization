defmodule StdOutEventListener do
  def start_link do
    spawn(fn -> run() end)
  end

  defp run do
    receive do
      event ->
        IO.puts event
    end

    run()
  end
end

defmodule NatVis do
  @moduledoc """
  Documentation for ElixirNativeVisualization.
  """

  @on_load :load_nif

  def load_nif do
    :erlang.load_nif("_build/libNatVis", 0)
  end

  def startRender(_eventlistener), do: raise "not implemented"
  def start do
    startRender StdOutEventListener.start_link()
  end

  def stopRender(_app), do: raise "not implemented"
end

